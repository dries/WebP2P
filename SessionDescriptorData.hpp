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
 * Filename:    SessionDescriptorData.hpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Description of the data structure for SDP.
 */

/* STL includes */
#include <string>
#include <vector>
#include <map>
 
enum NetType {
    INTERNET /* IN */ };

enum AddressType { 
    IP4 /* IP4 */, 
    IP6 /* IP6 */ };

enum CastType { 
    MULTICAST, 
    UNICAST };

enum BandwidthType { 
    CONFERENCE_TOTAL /* CT */, 
    APPLICATION_SPECIFIC /* AS */, 
    UNKNOWN /* ... */ };

enum EncryptionKeyMethod { 
    CLEAR, 
    BASE64, 
    URI, 
    PROMPT };

enum MediaType { 
    AUDIO, 
    VIDEO, 
    TEXT, 
    APPLICATION, 
    MESSAGE };
    
struct UnicastAddress {
    UnicastAddress() :
        isFQDN(true),
		address("localhost")
		{}
	bool        isFQDN;
	std::string address;
};

struct ConnectionAddress {
    ConnectionAddress() :
		castType(UNICAST),
		address("localhost"),
		ttl(0),
		numberOfAddresses(1)
		{}
	CastType      castType;
	std::string   address;
	unsigned char ttl;
	unsigned int  numberOfAddresses;
};

struct Origin {
	Origin() :
		username("-"),
		sessionID(0),
		sessioVersion(0),
		netType(INTERNET),
		addressType(IP4),
		unicastAddress()
		{}
	std::string    username;
	uint64_t       sessionID;
	uint64_t       sessioVersion;
	NetType        netType;
	AddressType    addressType;
	UnicastAddress unicastAddress;
};

struct ConnectionData {
	ConnectionData() :
		netType(INTERNET),
		addressType(IP4),
		connectionAddress()
		{}
	NetType           netType;
	AddressType       addressType;
	ConnectionAddress connectionAddress;
};

struct Bandwidth {
    Bandwidth() :
    	bandwidthType(APPLICATION_SPECIFIC),
		bandwidthTypeName("AS"),
		bandwidth(0)
		{}
	BandwidthType bandwidthType;
	std::string   bandwidthTypeName;
	unsigned int  bandwidth;
};

struct Timing {
    Timing() :
		startTime(0),
		stopTime(0),
		repeats()
		{}
	uint64_t                  startTime;
	uint64_t                  stopTime;
    
    /* r= */
        struct RepeatTimes {
        RepeatTimes() :
            repeatInterval(0),
            activeDuration(0),
            offsetsFromStartTime()
            {}
        uint64_t              repeatInterval;
        uint64_t              activeDuration;
        std::vector<uint64_t> offsetsFromStartTime;
    };

    std::vector<RepeatTimes>  repeats;
};



struct TimeZones {
    TimeZones() :
        adjustmentTimeAndOffsets()
        {}
    std::vector<std::pair<uint64_t, int64_t> > adjustmentTimeAndOffsets;
};

struct EncryptionKey {
    EncryptionKey() :
        method(BASE64),
		key("")
		{}
    EncryptionKeyMethod method;
    std::string         key;
};

struct MediaDefinition {
    MediaDefinition() :
        type(APPLICATION),
        port(49170),
        numberOfPorts(1),
        transportProtocol("udp"),
        format("0"),
        mediaInformation(),
        connectionData(),
        mediaAttributes()
        {}
    MediaType                          type;
    unsigned int                       port;
    unsigned int                       numberOfPorts;
	std::string                        transportProtocol;
    std::string                        format;

    /* i= */
	std::string                        mediaInformation;
	/* c= */
	std::vector<ConnectionData>        connectionData;
	/* k= */
	std::map<std::string, std::string> mediaAttributes;
};
	

struct SessionDescriptorData {
	SessionDescriptorData() :
		protocolVersion(0),
		origin(),
		sessionName(" "),
		sessionInformation(""),
		uri(""),
		emailAddresses(),
		phoneNumbers(),
		connectionData(),
		bandwidth(),
		timings(),
		timeZones(),
		encryptionKey(),
		sessionAttributes(),
		mediaDefinitions() {}
        
    /* v= */
	uint64_t                           protocolVersion;

	/* o= */
    Origin                             origin;
    
    /* s= */
	std::string                        sessionName;
    
    /* i= */
	std::string                        sessionInformation;
    
    /* u= */
	std::string                        uri;
    
    /* e= */
	std::vector<std::string>           emailAddresses;
	
	/* p= */
	std::vector<std::string>           phoneNumbers;
    
    /* c= */
    ConnectionData                     connectionData;
    
    /* b= */
    std::vector<Bandwidth>             bandwidth;
    
    /* t= */
   	std::vector<Timing>                timings;
    
    /* z= */
    TimeZones                          timeZones;
    
    /* k= */
    EncryptionKey                      encryptionKey;
    
	/* a= */
	std::map<std::string, std::string> sessionAttributes;
    
    /* m= */
    std::vector<MediaDefinition>       mediaDefinitions;
};

