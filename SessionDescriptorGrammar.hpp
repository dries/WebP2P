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

/* STL includes */
#include <sstream>

/* WebP2P includes */
#include "ABNFCoreGrammar.hpp"
#include "AddrSpecGrammar.hpp"
#include "URIReferenceGrammar.hpp"
 
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
 
struct SessionDescriptorGrammar :
    boost::spirit::qi::grammar<std::string::const_iterator> {
    /* external grammars */
    ABNFCoreGrammar     abnf;
    AddrSpecGrammar     addr_spec;
    URIReferenceGrammar URI_reference;
    
    /* rules for this grammar */
    boost::spirit::qi::rule<std::string::const_iterator>
        proto_version, origin_field, session_name_field, information_field,
        uri_field, email_fields, phone_fields, connection_field,
        bandwidth_fields, time_fields, repeat_fields, zone_adjustments,
        key_field, attribute_fields, media_descriptions, media_field, username,
        sess_id, sess_version, nettype, addrtype, uri, email_address,
        address_and_comment, dispname_and_address, phone_number, phone,
        connection_address, bwtype, bandwidth, start_time, stop_time, time,
        repeat_interval, typed_time, fixed_len_time_unit, key_type, base64_char,
        base64_unit, base64_pad, base64, attribute, att_field, att_value, media,
        fmt, proto, port, unicast_address, multicast_address, IP4_multicast,
        m1, IP6_multicast, bm, ttl, FQDN, IP4_address, b1, IP6_address, hexpart,
        hexseq, hex4, extn_addr, text, byte_string, non_ws_string, token_char,
        token, email_safe, integer, alpha_numeric, POS_DIGIT, decimal_uchar;

    /* start symbol for this grammar */
    boost::spirit::qi::rule<std::string::const_iterator/*, session_descriptor*/>
        session_description;
    
    SessionDescriptorGrammar();
};