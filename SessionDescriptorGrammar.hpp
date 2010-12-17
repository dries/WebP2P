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
 * Filename:    SessionDescriptorGrammar.hpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Definition for the RFC 4566 SDP grammar rules.
 * See:         http://tools.ietf.org/html/rfc4566
**/

#pragma once

/* STL includes */
#include <sstream>

/* WebP2P includes */
#include "ABNFCoreGrammar.hpp"
#include "AddrSpecGrammar.hpp"
#include "URIReferenceGrammar.hpp"
#include "SessionDescriptorGenericGrammar.hpp"
#include "SessionDescriptorData.hpp"
 
class ParseException : std::exception {
private:
    std::string errorDescription;
public:
    ParseException(const std::string& desc) {
        errorDescription = desc;
    }
    virtual ~ParseException() throw () {}
    
    virtual const char* what() const throw() {
        return errorDescription.c_str();
    }
};

using boost::spirit::qi::rule;

struct SessionDescriptorGrammar :
    boost::spirit::qi::grammar<std::string::const_iterator, SessionDescriptorData()> {
    typedef std::string::const_iterator it;

    /* external grammars */
    ABNFCoreGrammar                 abnf;
    AddrSpecGrammar                 addr_spec;
    URIReferenceGrammar             URI_reference;
    SessionDescriptorGenericGrammar sdp;
    
    /* rules for this grammar */
    rule<it, unsigned long long()>               proto_version;
    rule<it, Origin()>                           origin_field;
    rule<it, std::string()>                      session_name_field;
    rule<it, std::string()>                      information_field;
    rule<it, std::string()>                      uri_field;
    rule<it, std::vector<std::string>()>         email_fields;
    rule<it, std::vector<std::string>()>         phone_fields;
    rule<it, ConnectionData()>                   connection_field;
    rule<it, std::vector<Bandwidth>()>           bandwidth_fields;
    rule<it, std::vector<Time>()>                time_fields;
    rule<it/*, RepeatTimes() */>                     repeat_fields;
    rule<it/*, ZoneAdjustment()*/>                   zone_adjustments;
    rule<it/*, EncryptionKey()*/>                    key_field;
    rule<it, std::vector<Attribute>()>           attribute_fields;
    rule<it, std::vector<MediaDescription>()>    media_descriptions;
    rule<it, Media()>                            media_field;
    rule<it, std::string()>                      username;
    rule<it, unsigned long long()>               sess_id;
    rule<it, unsigned long long()>               sess_version;
    rule<it, std::string()>                      nettype;
    rule<it, std::string()>                      addrtype;
    rule<it, std::string()>                      uri;
    rule<it, std::string()>                      email_address;
    rule<it, std::string()>                      address_and_comment;
    rule<it, std::string()>                      dispname_and_address;
    rule<it, std::string()>                      phone_number;
    rule<it, std::string()>                      phone;
    rule<it, std::string()>                      connection_address;
    rule<it, std::string()>                      bwtype;
    rule<it, unsigned long long()>               bandwidth;
    rule<it, unsigned long long()>               start_time;
    rule<it, unsigned long long()>               stop_time;
    rule<it, unsigned long long()>               time;
    rule<it, TypedTime()>                        repeat_interval;
    rule<it, TypedTime()>                        typed_time;
    rule<it, std::string()>                      fixed_len_time_unit;
    rule<it, std::string()>                      key_type;
    rule<it, std::string()>                      base64_char;
    rule<it, std::string()>                      base64_unit;
    rule<it, std::string()>                      base64_pad;
    rule<it, std::string()>                      base64;
    rule<it, Attribute()>                        attribute;
    rule<it, std::string()>                      att_field;
    rule<it, std::string()>                      att_value;
    rule<it, std::string()>                      media;
    rule<it, std::string()>                      fmt;
    rule<it, std::string()>                      proto;
    rule<it, unsigned short()>                   port;

    /* start symbol for this grammar */
    rule<std::string::const_iterator, SessionDescriptorData()>
        session_description;
    
    SessionDescriptorGrammar();
};