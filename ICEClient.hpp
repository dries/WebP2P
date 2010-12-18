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
 * Description: Header for the class that implements a simplified view of the
 *              PJICE library.
**/

#pragma once

/* STL headers */
#include <string>
#include <vector>

/* Fix architecture detection for Win32 by forcing i386 */
#ifdef WIN32
    #define PJ_M_I386
#endif

/* PJPROJECT headers */
#include <pjnath.h>
#include <pjlib.h>
#include <pjlib-util.h>

class ICEClient {
    class ServerConfiguration {
    private:
        std::string stunServerName;
        int stunServerPort;

        std::string turnServerName;
        int turnServerPort;
        std::string turnUsername;
        std::string turnPassword;
    public:
        ServerConfiguration(const std::string& serverConfigString);
        
        const std::string& getStunServerName();
        int getStunServerPort();
        
        const std::string& getTurnServerName();
        int getTurnServerPort();
        const std::string& getTurnUsername();
        const std::string& getTurnPassword();
    };
    
    class RemoteConfiguration {
    public:
        std::string      ufrag;
        std::string      pwd;
        unsigned         comp_cnt;
        unsigned         cand_cnt;
        std::vector<pj_sockaddr> def_addr;
        std::vector<pj_ice_sess_cand> cand;
    };
public:
    class Callbacks {
    public:
        virtual void setLocalCandidates(const std::string& localCandidates) = 0;
        virtual void negotiationComplete() = 0;
        virtual void dataReceived(const std::string& text) = 0;
    };
private:
    /* PJNATH related stuff */
    pj_ice_strans_cfg ice_cfg;
    pj_ice_strans*    icest;
    pj_caching_pool   cp;
    pj_thread_t*      thread;
    pj_bool_t         thread_quit_flag;
    pj_pool_t*        pool;
    
    struct rem_info
    {
        std::string      ufrag;
        std::string      pwd;
        unsigned         comp_cnt;
        pj_sockaddr      def_addr[PJ_ICE_MAX_COMP];
        unsigned         cand_cnt;
        pj_ice_sess_cand cand[PJ_ICE_ST_MAX_CAND];
    } rem;
    
    ServerConfiguration     serverConfiguration;
    RemoteConfiguration     remoteConfiguration;
    
    unsigned           comp_cnt;
    
public:
    Callbacks* callbacks;

public:
    ICEClient(const std::string& server_cfg);
    ~ICEClient();
    
    pj_bool_t handleEvents(unsigned max_msec, unsigned *p_count);
    
    void setCallbacks(Callbacks* callbacks);
    void deliverLocalCandidates();
    void addRemoteCandidates(const std::string& remoteCandidates);
    
    void sendMessage(std::string message);
    
private:
    std::string initializeClient();
    void shutdownClient();
    void initializeTransport();
    void shutdownTransport();
    void initializeSession();
    void shutdownSession();
    
    std::string getLocalCandidates();
    void resetRemoteCandidates();
};