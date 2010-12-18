/**
 * This file is part of WebP2P.
 *
 * WebP2P is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * WebP2P is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WebP2P.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Filename:    ICEClient.hpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Implementation for the class that implements a simplified view
 *              of the PJICE library.
**/

/* STL includes */
#include <sstream>
#include <string>
#include <iostream>

/* Boost includes */
#include <boost/scoped_array.hpp>

/* WebP2P includes */
#include "ICEClient.hpp"
#include "SessionDescriptor.hpp"

ICEClient::ServerConfiguration::ServerConfiguration(
	const std::string& serverConfigString) {
    /* TODO: Parse configuration string here */
    /* Use hardcoded defaults for now */
    stunServerName = "numb.viagenie.ca";
    stunServerPort = PJ_STUN_PORT;
    turnServerName = "numb.viagenie.ca";
    turnServerPort = 3478;
    turnUsername = "dries.staelens@gmail.com";
    turnPassword = "numb123";
}

const std::string& ICEClient::ServerConfiguration::getStunServerName() {
    return stunServerName;
}

int ICEClient::ServerConfiguration::getStunServerPort() {
    return stunServerPort;
}

const std::string& ICEClient::ServerConfiguration::getTurnServerName() {
    return turnServerName;
}

int ICEClient::ServerConfiguration::getTurnServerPort() {
    return turnServerPort;
}

const std::string& ICEClient::ServerConfiguration::getTurnUsername() {
    return turnUsername;
}

const std::string& ICEClient::ServerConfiguration::getTurnPassword() {
    return turnPassword;
}

ICEClient::ICEClient(const std::string& server_cfg) :
    icest(NULL),
    thread(NULL),
    thread_quit_flag(PJ_FALSE),
    pool(NULL),
    serverConfiguration(server_cfg),
    comp_cnt(1) {
    initializeClient();
}

ICEClient::~ICEClient() {
    shutdownSession();
    shutdownTransport();
    shutdownClient();
}

static int worker_thread(void *ICEClient_instance) {
    while (!static_cast<ICEClient*>(ICEClient_instance)
    	->handleEvents(500, NULL)) {}

    return 0;
}

pj_bool_t ICEClient::handleEvents(unsigned max_msec, unsigned *p_count) {
    enum { MAX_NET_EVENTS = 1 };
    pj_time_val max_timeout = {0, 0};
    pj_time_val timeout = {0, 0};
    unsigned count = 0, net_event_count = 0;
    int c;
    
    max_timeout.msec = max_msec;
    
    /* Poll the timer to run it and also to retrieve the earliest entry. */
    timeout.sec = timeout.msec = 0;
    c = pj_timer_heap_poll( ice_cfg.stun_cfg.timer_heap, &timeout );
    if (c > 0)
        count += c;
    
    /* timer_heap_poll should never ever returns negative value, or otherwise
     * ioqueue_poll() will block forever!
     */
    pj_assert(timeout.sec >= 0 && timeout.msec >= 0);
    if (timeout.msec >= 1000) timeout.msec = 999;
    
    /* compare the value with the timeout to wait from timer, and use the 
     * minimum value. 
     */
    if (PJ_TIME_VAL_GT(timeout, max_timeout))
        timeout = max_timeout;
    
    /* Poll ioqueue. 
     * Repeat polling the ioqueue while we have immediate events, because
     * timer heap may process more than one events, so if we only process
     * one network events at a time (such as when IOCP backend is used),
     * the ioqueue may have trouble keeping up with the request rate.
     *
     * For example, for each send() request, one network event will be
     *   reported by ioqueue for the send() completion. If we don't poll
     *   the ioqueue often enough, the send() completion will not be
     *   reported in timely manner.
     */
    do {
        c = pj_ioqueue_poll(ice_cfg.stun_cfg.ioqueue, &timeout);
        if (c < 0) {
            //pj_status_t err = pj_get_netos_error();
            pj_thread_sleep(PJ_TIME_VAL_MSEC(timeout));
            if (p_count)
                *p_count = count;
            return thread_quit_flag;
        } else if (c == 0) {
            break;
        } else {
            net_event_count += c;
            timeout.sec = timeout.msec = 0;
        }
    } while (c > 0 && net_event_count < MAX_NET_EVENTS);
    
    count += net_event_count;
    if (p_count)
        *p_count = count;
    
    return thread_quit_flag;
}

/*
 * This is the callback that is registered to the ICE stream transport to
 * receive notification about incoming data. By "data" it means application
 * data such as RTP/RTCP, and not packets that belong to ICE signaling (such
 * as STUN connectivity checks or TURN signaling).
 */
static void cb_on_rx_data(pj_ice_strans *ice_st,
                          unsigned comp_id, 
                          void *pkt, pj_size_t size,
                          const pj_sockaddr_t *src_addr,
                          unsigned src_addr_len)
{
    //char ipstr[PJ_INET6_ADDRSTRLEN+10];
    
    PJ_UNUSED_ARG(ice_st);
    PJ_UNUSED_ARG(src_addr_len);
    PJ_UNUSED_ARG(pkt);
    
    std::string receivedText((const char*)pkt, size);
    static_cast<ICEClient*>(pj_ice_strans_get_user_data(ice_st))->
            callbacks->dataReceived(receivedText);
    
    // Don't do this! It will ruin the packet buffer in case TCP is used!
    //((char*)pkt)[size] = '\0';
    /*
    PJ_LOG(3,(THIS_FILE, "Component %d: received %d bytes data from %s: \"%.*s\"",
              comp_id, size,
              pj_sockaddr_print(src_addr, ipstr, sizeof(ipstr), 3),
              (unsigned)size,
              (char*)pkt));*/
}

/*
 * This is the callback that is registered to the ICE stream transport to
 * receive notification about ICE state progression.
 */
static void cb_on_ice_complete(pj_ice_strans *ice_st, 
                               pj_ice_strans_op op,
                               pj_status_t status)
{
    if(op == PJ_ICE_STRANS_OP_INIT) {
        /* Initialization (candidate gathering) */
        static_cast<ICEClient*>(pj_ice_strans_get_user_data(ice_st))
        	->deliverLocalCandidates();
        std::cout << "ICEClient: ICE Transport Initiated";
    }
    else if(op == PJ_ICE_STRANS_OP_NEGOTIATION) {
        /* Negotiation */
        std::cout << "ICEClient: ICE Negotiation Complete";
        static_cast<ICEClient*>(pj_ice_strans_get_user_data(ice_st))->
            callbacks->negotiationComplete();
    }
    else if(op == PJ_ICE_STRANS_OP_KEEP_ALIVE) {
        /* This operation is used to report failure in keep-alive operation. */
        /* Currently it is only used to report TURN Refresh failure. */
        std::cout << "ICEClient: Keep-alive failed";
    }
    else {
        /* Error */
        std::cout << "ICEClient: Error in PJNATH library";
    }
}

std::string ICEClient::initializeClient() {
    pj_status_t status;
    
    /* Initialize libraries */
    status = pj_init();
    if(status != PJ_SUCCESS) {
        return std::string("pj_init() failed");
    }
    status = pjlib_util_init();
    if(status != PJ_SUCCESS) {
        return std::string("pjlib_util_init() failed");
    }
    status = pjnath_init();
    if(status != PJ_SUCCESS) {
        return std::string("pjnath_init() failed");
    }
    
    /* Init ICE settings with default values */
    pj_ice_strans_cfg_default(&ice_cfg);
    pj_stun_sock_cfg_default(&ice_cfg.stun.cfg);
    pj_turn_sock_cfg_default(&ice_cfg.turn.cfg);
    
    /* Create the caching pool factory */
    pj_caching_pool_init(&cp, NULL, 0);
    ice_cfg.stun_cfg.pf = &cp.factory;
    
    /* Create memory pool */
    pool = pj_pool_create(&cp.factory, "web_p2p", 512, 512, NULL);
    if(pool == NULL) {
        return std::string("pj_pool_create() failed");
    }
    
    /* Create timer heap */
    status = pj_timer_heap_create(pool, 100, &ice_cfg.stun_cfg.timer_heap);
    if(status != PJ_SUCCESS) {
        return std::string("pj_timer_heap_create() failed");
    }
    
    /* Create ioqueue */
    status = pj_ioqueue_create(pool, 16, &ice_cfg.stun_cfg.ioqueue);
    if(status != PJ_SUCCESS) {
        return std::string("pj_ioqueue_create() failed");
    }
    
    /* Create event polling thread */
    status = pj_thread_create(
    	pool, "web_p2p", &worker_thread, this, 0, 0, &thread);
    if(status != PJ_SUCCESS) {
        return std::string("pj_thread_create() failed");
    }
    
    /* Create DNS resolver */
    pj_str_t dnsServer;
    pj_cstr(&dnsServer, "8.8.8.8");
    status = pj_dns_resolver_create(
    	&cp.factory, "resolver", 0, ice_cfg.stun_cfg.timer_heap,
        ice_cfg.stun_cfg.ioqueue, &ice_cfg.resolver);
    if(status != PJ_SUCCESS) {
        return std::string("pj_dns_resolver() failed");
    }
    status = pj_dns_resolver_set_ns(ice_cfg.resolver, 1, &dnsServer, NULL);
    if(status != PJ_SUCCESS) {
        return std::string("pj_dns_resolver_set_ns() failed");
    }
    
    /* Maximum number of host candidates */
    ice_cfg.stun.max_host_cands = PJ_ICE_ST_MAX_CAND;
    
    /* Configure agressive nomination */
    ice_cfg.opt.aggressive = PJ_FALSE;
    
    /* Configure STUN server/port/keep-alive interval */
    if(serverConfiguration.getStunServerName().length() > 0) {
        pj_cstr(&ice_cfg.stun.server,
        	serverConfiguration.getStunServerName().c_str());
        ice_cfg.stun.port = serverConfiguration.getStunServerPort();
        ice_cfg.stun.cfg.ka_interval = 300;
    }
    
    /* Configure TURN server */
    if(serverConfiguration.getTurnServerName().length()) {
        pj_cstr(&ice_cfg.turn.server,
        	serverConfiguration.getTurnServerName().c_str());
        ice_cfg.turn.port = serverConfiguration.getTurnServerPort();
        ice_cfg.turn.conn_type = PJ_TURN_TP_UDP;
        ice_cfg.turn.alloc_param.ka_interval = 300;
    }
    
    /* TURN credentials */
    if(serverConfiguration.getTurnUsername().length()) {
        ice_cfg.turn.auth_cred.type = PJ_STUN_AUTH_CRED_STATIC;
        pj_cstr(&ice_cfg.turn.auth_cred.data.static_cred.username,
        	serverConfiguration.getTurnUsername().c_str());
        ice_cfg.turn.auth_cred.data.static_cred.data_type 
        	= PJ_STUN_PASSWD_PLAIN;
        pj_cstr(&ice_cfg.turn.auth_cred.data.static_cred.data,
        	serverConfiguration.getTurnPassword().c_str());
    }

    return std::string("initializeClient() succeeded");
}

void ICEClient::shutdownClient() {
    if(icest)
        pj_ice_strans_destroy(icest);

    pj_thread_sleep(500);
    thread_quit_flag = PJ_TRUE;
    if(thread) {
        pj_thread_join(thread);
        pj_thread_destroy(thread);
    }
    
    if(ice_cfg.stun_cfg.ioqueue)
        pj_ioqueue_destroy(ice_cfg.stun_cfg.ioqueue);

    if(ice_cfg.stun_cfg.timer_heap)
        pj_timer_heap_destroy(ice_cfg.stun_cfg.timer_heap);
    
    pj_caching_pool_destroy(&cp);
    
    pj_shutdown();
}

void ICEClient::initializeTransport() {
    if(NULL != icest)
        return;
    
    /* init the callback */
    pj_ice_strans_cb icecb;
    pj_bzero(&icecb, sizeof(icecb));
    icecb.on_rx_data = cb_on_rx_data;
    icecb.on_ice_complete = cb_on_ice_complete;
    
    /* create the instance */
    pj_status_t status = pj_ice_strans_create("web_p2p",
    	&ice_cfg, comp_cnt,	this, &icecb, &icest);
    if(status != PJ_SUCCESS)
        return;
    return;
}

void ICEClient::shutdownTransport() {
    if(NULL == icest)
        return;
    
    pj_ice_strans_destroy(icest);
    icest = NULL;
    resetRemoteCandidates();
}

void ICEClient::initializeSession() {
    if(icest == NULL)
        initializeTransport();
    if(pj_ice_strans_has_sess(icest))
        return;
    
    //pj_ice_sess_role role = PJ_ICE_SESS_ROLE_CONTROLLING;
    pj_ice_sess_role role = PJ_ICE_SESS_ROLE_CONTROLLED;
    pj_status_t status;
        
    status = pj_ice_strans_init_ice(icest, role, NULL, NULL);
    if (status != PJ_SUCCESS) {
        return;
    }

    resetRemoteCandidates();    
}

void ICEClient::shutdownSession() {
    if(icest == NULL)
        return;
    if(!pj_ice_strans_has_sess(icest))
        return;
    
    pj_ice_strans_stop_ice(icest);
    resetRemoteCandidates();
}

void ICEClient::setCallbacks(Callbacks* callbacks) {
    if(NULL == icest)
        initializeTransport();
//    if(!pj_ice_strans_has_sess(icest))
//        initializeSession();    
    this->callbacks = callbacks;
}

void ICEClient::deliverLocalCandidates() {
    if(!pj_ice_strans_has_sess(icest))
        initializeSession(); 

    std::string localCandidates = getLocalCandidates();
    callbacks->setLocalCandidates(localCandidates);
}

std::string ICEClient::getLocalCandidates() {
    pj_status_t status;
    
    pj_str_t local_ufrag, local_pwd;
    status = pj_ice_strans_get_ufrag_pwd(icest, &local_ufrag, &local_pwd, 
    	NULL, NULL);
    if(status != PJ_SUCCESS)
        return std::string("pj_ice_strans_get_ufrag_pwd() failed");
    
    std::stringstream candidateList;
    candidateList << "v=0\no=- 3414953978 3414953978 IN IP4 localhost\ns=ice\nt=0 0\n";
    candidateList << "a=ice-ufrag:";
    candidateList.write(local_ufrag.ptr, local_ufrag.slen);
    candidateList << "\na=ice-pwd:";
    candidateList.write(local_pwd.ptr, local_pwd.slen);
    candidateList << "\n";

    
    for(unsigned comp = 1; comp <= comp_cnt; ++comp) {
        char ipaddr[PJ_INET6_ADDRSTRLEN];
        unsigned cand_cnt;
        pj_ice_sess_cand cand[PJ_ICE_ST_MAX_CAND];

        status = pj_ice_strans_get_def_cand(icest, comp, &cand[0]);
        if(status != PJ_SUCCESS)
            return std::string("pj_ice_strans_get_def_cand() failed");

        if(comp == 1) {
            candidateList << "m=audio ";
            candidateList << (unsigned)pj_sockaddr_get_port(&cand[0].addr);
            candidateList << " RTP/AVP 0\nc=IN IP4 ";
            candidateList 
            	<< pj_sockaddr_print(&cand[0].addr, ipaddr, sizeof(ipaddr), 0);
            candidateList << "\n";
        }
        else if(comp == 2) {
            candidateList << "m=rtcp:";
            candidateList << (unsigned)pj_sockaddr_get_port(&cand[0].addr);
            candidateList << " IN IP4 ";
            candidateList 
            	<< pj_sockaddr_print(&cand[0].addr, ipaddr, sizeof(ipaddr), 0);
            candidateList << "\n";
        }
        else {
            candidateList << "a=Xice-defcand:";
            candidateList << (unsigned)pj_sockaddr_get_port(&cand[0].addr);
            candidateList << " IN IP4 ";
            candidateList 
            	<< pj_sockaddr_print(&cand[0].addr, ipaddr, sizeof(ipaddr), 0);
            candidateList << "\n";
        }

        status = pj_ice_strans_enum_cands(icest, comp, &cand_cnt, cand);
        if(status != PJ_SUCCESS)
            return std::string("pj_ice_strans_enum_cands() failed");
    
        for(unsigned j = 0; j < cand_cnt; ++j) {
            candidateList << "a=candidate:";
            candidateList.write(
            	cand[j].foundation.ptr, cand[j].foundation.slen);
            candidateList 
            	<< " " 
            	<< (unsigned) cand[j].comp_id 
            	<< " UDP " 
            	<< cand[j].prio 
            	<< " ";
            candidateList 
            	<< pj_sockaddr_print(&cand[j].addr, ipaddr, sizeof(ipaddr), 0);
            candidateList 
            	<< " " 
            	<< (unsigned) pj_sockaddr_get_port(&cand[j].addr) 
            	<< " typ ";
            candidateList << pj_ice_get_cand_type_name(cand[j].type);
            candidateList << "\n";
        }
    }

    return candidateList.str();
}

void ICEClient::addRemoteCandidates(const std::string& remoteCandidates) {
    try {
        SessionDescriptor remoteSDP = SessionDescriptor(remoteCandidates);
        SessionDescriptorData& sdp = remoteSDP.getData();
        for(std::vector<Attribute>::iterator i = sdp.sessionAttributes.begin();
            i != sdp.sessionAttributes.end(); i++) {
            if(ValuedAttribute* at = boost::get<ValuedAttribute>(&*i)) {
                std::string attributeName = boost::fusion::at_c<0>(*at);
                std::string attributeValue = boost::fusion::at_c<1>(*at);
                if(attributeName ==  "ice-ufrag")
                    remoteConfiguration.ufrag = attributeValue;
                else if(attributeName == "ice-pwd")
                    remoteConfiguration.pwd = attributeValue;
            }
        }

        remoteConfiguration.comp_cnt = 0;
        for(std::vector<MediaDescription>::iterator i = sdp.mediaDescriptions.begin();
            i != sdp.mediaDescriptions.end(); i++) {
            remoteConfiguration.comp_cnt++;
            
            if(i->connectionData.size() >= 1)
            {
                ConnectionData& cdata = i->connectionData[0];
                if(cdata.netType == "IN")
                {
                    int af = (cdata.addressType == "IP4") ?
                        pj_AF_INET() : pj_AF_INET6();
                    pj_sockaddr addr;
                    pj_sockaddr_init(af, &addr, NULL, 0);
                    pj_str_t temp;
                    pj_cstr(&temp, cdata.connectionAddress.data());
                    pj_sockaddr_set_str_addr(af, &addr, &temp);
                    remoteConfiguration.def_addr.push_back(addr);
                }
            }

            for(std::vector<Attribute>::iterator j = i->mediaAttributes.begin();
                j != i->mediaAttributes.end(); j++) {
                if(ValuedAttribute* at = boost::get<ValuedAttribute>(&*j)) {
                    std::string attributeName = boost::fusion::at_c<0>(*at);
                    std::string attributeValue = boost::fusion::at_c<1>(*at);
                    if(attributeName ==  "candidate") {
                        std::stringstream ss(attributeValue);
                        pj_ice_sess_cand cand;
                        std::string foundation;
                        int comp_id;
                        std::string transport;
                        int prio;
                        std::string ipaddr;
                        unsigned short port;
                        std::string skip, type;
                        ss >> foundation
                           >> comp_id
                           >> transport
                           >> prio
                           >> ipaddr
                           >> port
                           >> skip
                           >> type;
                        if(type == "host") {
                            cand.type = PJ_ICE_CAND_TYPE_HOST;
                        } else if(type == "srflx") {
                            cand.type = PJ_ICE_CAND_TYPE_SRFLX;
                        } else if(type == "relay") {
                            cand.type = PJ_ICE_CAND_TYPE_RELAYED;
                        }
                        cand.comp_id = (pj_uint8_t) comp_id;
                        cand.prio    = prio;
                        pj_strdup2(pool, &cand.foundation, foundation.c_str());
                        
                        int af;
                        if(ipaddr.find(":") == std::string::npos) {
                            af = pj_AF_INET();
                        } else {
                            af = pj_AF_INET6();
                        }
                        
                        pj_str_t temp;
                        pj_cstr(&temp, ipaddr.c_str());
                        pj_sockaddr_init(af, &cand.addr, NULL, 0);
                        pj_sockaddr_set_str_addr(af, &cand.addr, &temp);
                        pj_sockaddr_set_port(&cand.addr, (pj_uint16_t)port);
                        remoteConfiguration.cand.push_back(cand);
                    }
                }
            }
        }
        pj_str_t rufrag, rpwd;
        pj_status_t status;
        
        int cand_cnt = remoteConfiguration.cand.size();        
        boost::scoped_array<pj_ice_sess_cand> cand(new pj_ice_sess_cand[cand_cnt]);
        for(int i = 0; i < cand_cnt; i++) {
            cand[i] = remoteConfiguration.cand[i];
        }
        status = pj_ice_strans_start_ice(icest, 
			pj_cstr(&rufrag, remoteConfiguration.ufrag.c_str()),
			pj_cstr(&rpwd, remoteConfiguration.pwd.c_str()),
			cand_cnt,
			cand.get());
        
    } catch (std::exception) {
    }
}

void ICEClient::sendMessage(std::string message) {
    pj_status_t status;
    
    status = pj_ice_strans_sendto(
        icest,
        1,
        message.c_str(),
        message.length(),
        &remoteConfiguration.def_addr[0],
        pj_sockaddr_get_len(&remoteConfiguration.def_addr[0]));
}

void ICEClient::resetRemoteCandidates() {
    pj_bzero(&rem, sizeof(rem));
}