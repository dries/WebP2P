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
 * Filename:    SessionDescriptor.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Implementation of the SessionDescriptor class which implements
 *              a generic SDP message.
 * See also:    http://tools.ietf.org/html/rfc4566
**/

/* Boost includes */
#include <boost/variant/get.hpp>

/* WebP2P includes */
#include "SessionDescriptor.hpp"
#include "SessionDescriptorGrammar.hpp"

SessionDescriptor::SessionDescriptor() {
}

SessionDescriptor::SessionDescriptor(const std::string& sdpString)
    throw(std::exception) {
    using namespace boost::spirit::qi;

    std::string::const_iterator begin = sdpString.begin();
    std::string::const_iterator end   = sdpString.end();
    SessionDescriptorGrammar sdpGrammar;
    bool success;
    
    std::cout << "Attempting to parse the following SDP string:\n"
              << sdpString;

    try {
        success = phrase_parse(begin, end, sdpGrammar, !byte_, data);
    } catch (std::exception ex) {
        std::cout << "Parse exception: " << ex.what() << "\n";
    }
    bool fullMatch = (begin == end);

    if(fullMatch) {
        std::cout << "Full match found!\n";
        std::cout << std::string(*this);

    }
    else if(success) { 
        std::cout << "Partial match found!\n";
    }
    else {
        std::cout << "No match found!\n";
    }
}

SessionDescriptor::~SessionDescriptor() {
}

SessionDescriptor::operator std::string() {
    std::stringstream ss;
    /* v= */
    ss << "v=" << data.protocolVersion       << "\n";

    /* o= */
    ss << "o=" << data.origin.username       << " "
               << data.origin.sessionID      << " "
               << data.origin.sessionVersion << " "
               << data.origin.netType        << " "
               << data.origin.addressType    << " "
               << data.origin.unicastAddress << "\n";

    /* s= */
    ss << "s=" << data.sessionName           << "\n";
    
    /* t= */
    for(std::vector<Time>::iterator i = data.time.begin();
        i != data.time.end(); i++) {
        ss << "t=" << i->startTime           << " "
                   << i->stopTime            << "\n";
    }

    /* a= */
    for(std::vector<Attribute>::iterator i = data.sessionAttributes.begin();
        i != data.sessionAttributes.end(); i++) {
        Attribute &v = *i;
        if(PropertyAttribute* at = boost::get<PropertyAttribute>(&v))
            ss << "a=" << *at                         << "\n";
        else if(ValuedAttribute* at = boost::get<ValuedAttribute>(&v))
            ss << "a=" << boost::fusion::at_c<0>(*at) << ":"
                       << boost::fusion::at_c<1>(*at) << "\n";
    }

    for(std::vector<MediaDescription>::iterator i = data.mediaDescriptions.begin();
        i != data.mediaDescriptions.end(); i++) {

        /* m= */
        ss << "m=" << i->media.mediaName << " "
                   << i->media.port      << " "
                   << i->media.protocolName;
        for(std::vector<std::string>::iterator j = i->media.formats.begin();
            j != i->media.formats.end(); j++) {
            ss << " " << *j;
        }
        ss << "\n";

        /* c= */
        for(std::vector<ConnectionData>::iterator j = i->connectionData.begin();
            j != i->connectionData.end(); j++) {
            ss << "c=" << j->netType           << " "
                       << j->addressType       << " "
                       << j->connectionAddress << "\n";
        }

        /* a= */
        for(std::vector<Attribute>::iterator j
            = i->mediaAttributes.begin();
            j != i->mediaAttributes.end(); j++) {
            Attribute &v = *j;
            if(PropertyAttribute* at = boost::get<PropertyAttribute>(&v))
                ss << "a=" << *at << "\n";
            else if(ValuedAttribute* at = boost::get<ValuedAttribute>(&v))
                ss << "a=" << boost::fusion::at_c<0>(*at) << ":"
                           << boost::fusion::at_c<1>(*at) << "\n";
        }
    }
    return ss.str();
}

SessionDescriptorData& SessionDescriptor::getData() {
    return data;
}