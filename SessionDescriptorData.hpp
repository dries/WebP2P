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
**/

#pragma once

/* STL includes */
#include <string>
#include <vector>
#include <map>

/* Boost includes */
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/variant.hpp>

 
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
	bool        isFQDN;
	std::string address;
};

struct ConnectionAddress {
	CastType      castType;
	std::string   address;
	unsigned char ttl;
	unsigned int  numberOfAddresses;
};

struct Origin {
	std::string        username;
	unsigned long long sessionID;
	unsigned long long sessionVersion;
	std::string        netType;
	std::string        addressType;
	std::string        unicastAddress;
};

BOOST_FUSION_ADAPT_STRUCT(
    Origin,
    (std::string,        username)
    (unsigned long long, sessionID)
    (unsigned long long, sessionVersion)
    (std::string,        netType)
    (std::string,        addressType)
    (std::string,        unicastAddress)
)

struct ConnectionData {
	std::string       netType;
	std::string       addressType;
	std::string       connectionAddress;
};

BOOST_FUSION_ADAPT_STRUCT(
    ConnectionData,
    (std::string,     netType)
    (std::string,     addressType)
    (std::string,     connectionAddress)
)

struct Bandwidth {
	BandwidthType      bandwidthType;
	std::string        bandwidthTypeName;
	unsigned long long bandwidth;
};

BOOST_FUSION_ADAPT_STRUCT(
    Bandwidth,
    (std::string,        bandwidthTypeName)
    (unsigned long long, bandwidth)
)

struct TypedTime {
    unsigned long long time;
    std::string        timeUnit;
};

BOOST_FUSION_ADAPT_STRUCT(
    TypedTime,
    (unsigned long long, time)
    (std::string,        timeUnit)
)

struct Time {
	unsigned long long startTime;
	unsigned long long stopTime;
    
    /* r= */
//    std::vector<RepeatTimes>  repeats;
};

BOOST_FUSION_ADAPT_STRUCT(
    Time,
    (unsigned long long, startTime)
    (unsigned long long, stopTime)
)

struct RepeatTimes {
    unsigned long long              repeatInterval;
    unsigned long long              activeDuration;
    std::vector<unsigned long long> offsetsFromStartTime;
};

typedef boost::fusion::vector<unsigned long long, long long> ZoneAdjustment;

struct EncryptionKey {
    std::string         methodName;
    std::string         key;
};

BOOST_FUSION_ADAPT_STRUCT(
    EncryptionKey,
    (std::string, methodName)
    (std::string, key)
)

typedef boost::fusion::vector<std::string, std::string> ValuedAttribute;
typedef std::string PropertyAttribute;
typedef boost::variant<ValuedAttribute, PropertyAttribute> Attribute;

struct Media {
    std::string                 mediaName;
    unsigned short              port;
//    unsigned short              numberOfPorts;
    std::string                 protocolName;
    std::vector<std::string>    formats;
};

BOOST_FUSION_ADAPT_STRUCT(
    Media,
    (std::string, mediaName)
    (unsigned short, port)
    (std::string, protocolName)
    (std::vector<std::string>, formats)
)

struct MediaDescription {
    /* m= */
    Media                              media;
    
    /* i= */
//	std::string                        mediaInformation;
	/* c= */
	std::vector<ConnectionData>        connectionData;
	/* k= */
	std::vector<Attribute>             mediaAttributes;
};

BOOST_FUSION_ADAPT_STRUCT(
    MediaDescription,
    (Media, media)
    (std::vector<ConnectionData>, connectionData)
    (std::vector<Attribute>, mediaAttributes)
)
	

struct SessionDescriptorData {
    /* v= */
	unsigned long long                 protocolVersion;

	/* o= */
    Origin                             origin;
    
    /* s= */
	std::string                        sessionName;
    
    /* i= */
//	std::string                        sessionInformation;
    
    /* u= */
//	std::string                        uri;
    
    /* e= */
//	std::vector<std::string>           emailAddresses;
	
	/* p= */
//	std::vector<std::string>           phoneNumbers;
    
    /* c= */
//    ConnectionData                     connectionData;
    
    /* b= */
//    std::vector<Bandwidth>             bandwidth;
    
    /* t= */
   	std::vector<Time>                  time;
    
    /* z= */
//    ZoneAdjustments                    zoneAdjustments;
    
    /* k= */
//    EncryptionKey                      encryptionKey;
    
	/* a= */
	std::vector<Attribute>             sessionAttributes;
    
    /* m= */
    std::vector<MediaDescription>      mediaDescriptions;
};

BOOST_FUSION_ADAPT_STRUCT(
    SessionDescriptorData,
    (unsigned long long, protocolVersion)
    (Origin, origin)
    (std::string, sessionName)
    (std::vector<Time>, time)
    (std::vector<Attribute>, sessionAttributes)
  (std::vector<MediaDescription>, mediaDescriptions)
)

