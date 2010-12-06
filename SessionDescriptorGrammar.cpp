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
 * Filename:    SessionDescriptorGrammar.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Implementation for the RFC 4566 SDP grammar rules.
 * See:         http://tools.ietf.org/html/rfc4566
**/

/* STL includes */
#include <sstream>

/* Boost includes */
#include <boost/spirit/include/phoenix_algorithm.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_scope.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>

/* WebP2P includes */
#include "SessionDescriptorData.hpp"
#include "SessionDescriptorGrammar.hpp"

SessionDescriptorGrammar::SessionDescriptorGrammar() :
    SessionDescriptorGrammar::base_type(session_description) {
    using           boost::spirit::qi::ascii::string;
    using           boost::spirit::qi::ascii::char_;
    using namespace boost::spirit::qi::ascii;
    using namespace boost::spirit::qi;
    using namespace boost::phoenix;
    using namespace boost::phoenix::local_names;

    /* Define error handler */
    on_error<fail>
    (
        session_description,
        let(_s = construct<std::string>())
        [
            _s += val("Error! Expecting "),
            //_s += _4,
            _s += val(" here: \""),
            _s += construct<std::string>(_3, _2),
            _s += val("\"\n"),
            throw_(construct<ParseException>(_s))/*
            let(_x = construct<ParseException>(_s))
            [
                throw_(_x)
            ]*/
        ]
    );
    
    /* The start symbol */
    session_description  = proto_version
                        >> origin_field
                        >> session_name_field
                        >> information_field
                        >> uri_field
                        >> email_fields
                        >> phone_fields
                        >> connection_field
                        >> bandwidth_fields
                        >> time_fields
                        >> key_field
                        >> attribute_fields
                        >> media_descriptions;

    /* SDP fields */
    proto_version        =   lit("v=")
                        >> +abnf.DIGIT >> (abnf.CRLF | abnf.LF);
    origin_field         =   lit("o=")
                        >> username >> abnf.SP
                        >> sess_id >> abnf.SP
                        >> sess_version >> abnf.SP
                        >> nettype >> abnf.SP
                        >> addrtype >> abnf.SP
                        >> unicast_address >> (abnf.CRLF | abnf.LF);
    session_name_field   =   lit("s=")
                        >> text >> (abnf.CRLF | abnf.LF);
    information_field    = -(lit("i=")
                        >> text >> (abnf.CRLF | abnf.LF));
    uri_field            = -(lit("u=")
                        >> uri >> (abnf.CRLF | abnf.LF));
    email_fields         = *(lit("e=")
                        >> email_address >> (abnf.CRLF | abnf.LF));
    phone_fields         = *(lit("p=")
                        >> phone_number >> (abnf.CRLF | abnf.LF));
    connection_field     = -(lit("c=")
                        >> nettype >> abnf.SP
                        >> addrtype >> abnf.SP
                        >> connection_address >> (abnf.CRLF | abnf.LF));
                        // a connection field must be present in every media
                        // description or at the session-level
    bandwidth_fields     = *(lit("b=")
                        >> bwtype >> lit(":") 
                        >> bandwidth >> (abnf.CRLF | abnf.LF));
    time_fields          = +(lit("t=")
                        >> start_time >> abnf.SP >> stop_time 
                        >> *((abnf.CRLF | abnf.LF) >> repeat_fields)
                        >> (abnf.CRLF | abnf.LF))
                        >> -(zone_adjustments >> (abnf.CRLF | abnf.LF));
    repeat_fields        =   lit("r=")
                        >> repeat_interval >> abnf.SP
                        >> typed_time >> +(abnf.SP >> typed_time);
    zone_adjustments     =   lit("z=")
                        >> time >> abnf.SP 
                        >> -lit("-") >> typed_time 
                        >> *(abnf.SP >> time 
                        >> abnf.SP >> -lit("-") >> typed_time);
    key_field            = -(lit("k=")
                        >> key_type >> (abnf.CRLF | abnf.LF));
    attribute_fields     = *(lit("a=")
                        >> attribute >> (abnf.CRLF | abnf.LF));
    media_descriptions   = *(media_field
                        >> information_field
                        >> *connection_field
                        >> bandwidth_fields
                        >> key_field
                        >> attribute_fields);
    media_field          =   lit("m=")
                        >> media >> abnf.SP 
                        >> port >> -(lit('/') >> integer) >> abnf.SP 
                        >> proto >> +(abnf.SP >> fmt) >> (abnf.CRLF | abnf.LF);                           

    /* sub-rules of 'o=' */
    username             = non_ws_string;
                        // pretty wide definition, but doesn't include space
    sess_id              = +abnf.DIGIT;
                        // should be unique for this username/host
    sess_version         = +abnf.DIGIT;
    nettype              = token;
                        // typically "IN"
    addrtype             = token;
                        // typically "IP4" or "IP6"

    /* sub-rules of 'u=' */
    uri                  = URI_reference; // see RFC 3986

    /* sub-rules of 'e=', see RFC 2822 for definitions */
    email_address        = address_and_comment 
                         | dispname_and_address
                         | addr_spec;
    address_and_comment  = addr_spec >> +abnf.SP 
                        >> char_('(') >> +email_safe >> char_(')');
    dispname_and_address = +email_safe >> +abnf.SP
                        >> char_('<') >> addr_spec >> char_('>');

    /* sub-rules of 'p=' */
    phone_number         = (phone >> *abnf.SP >> char_('(') 
                        >> +email_safe >> char_(')')) 
                         | (+email_safe >> char_('<') >> phone >> char_('>'))
                         | phone;
    phone                = -char_('+') >> abnf.DIGIT 
                        >> +(abnf.SP | char_('-') | abnf.DIGIT);

    /* sub-rules of 'c=' */
    connection_address   =  multicast_address | unicast_address;

    /* sub-rules of 'b=' */
    bwtype               = token;
    bandwidth            = +abnf.DIGIT;

    /* sub-rules of 't=' */
    /* Decimal representation of NTP time in seconds since 1900.  The
     * representation of NTP time is an unbounded length field containing at
     * least 10 digits.  Unlike the 64-bit representation used elsewhere, time
     * in SDP does not wrap in the year 2036. -- ANNOYING!
     */
    start_time           = time | char_('0');
    stop_time            = time | char_('0');
    time                 = POS_DIGIT >> repeat(9, inf)[abnf.DIGIT];

    /* sub-rules of 'r=' and 'z=' */
    repeat_interval      = POS_DIGIT >> *abnf.DIGIT >> -fixed_len_time_unit;
    typed_time           = +abnf.DIGIT >> -fixed_len_time_unit;
    fixed_len_time_unit  = char_('d') | char_('h') | char_('m') | char_('s');

    /* sub-rules of 'k=' */
    key_type             = string("prompt") 
                         | string("clear:") 
                         | string("base64:")
                         | string("uri:");
    base64_char          = abnf.ALPHA 
                         | abnf.DIGIT
                         | char_('+') 
                         | char_('/');
    base64_unit          = repeat(4)[base64_char];
    base64_pad           = repeat(2)[base64_char] >> repeat(2)[char_('=')] 
                         | repeat(3)[base64_char] >> char_('=');
    base64               = *base64_unit >> -base64_pad;

    /* sub-rules of 'a=' */
    attribute            = (att_field >> char_(':') >> att_value) | att_field;
    att_field            = token;
    att_value            = byte_string;

    /* sub-rules of 'm=' */
    media                = token;
                        // typically "audio", "video", "text", or "application"
    fmt                  = token;
                        // typically an RTP payload type for audio and video
                        // media
    proto                = token >> *(char_('/') >> token); 
                        // typically "RTP/AVP" or "udp"
    port                 = +abnf.DIGIT;

    /* generic sub-rules: addressing */
    unicast_address      = IP4_address | IP6_address | FQDN | extn_addr;
    multicast_address    = IP4_multicast | IP6_multicast | FQDN | extn_addr;
    IP4_multicast        = m1 >> repeat(3)[char_('.') >> decimal_uchar]
                        >> char_('/') >> ttl >> -(char_('/') >> integer);
                        // IPv4 multicast addresses may be in the
                        // range 224.0.0.0 to 239.255.255.255
    m1                   = (char_('2') >> char_('2') >> char_('4', '9'))
                         | (char_('2') >> char_('3') > abnf.DIGIT);
    IP6_multicast        = hexpart >> -(char_('/') >> integer);
                        // IPv6 address starting with FF
    bm                   = abnf.DIGIT
                         | (POS_DIGIT >> abnf.DIGIT) 
                         | (char_('1') >> repeat(2)[abnf.DIGIT]) 
                         | (char_('2') >> char_('0', '1') >> abnf.DIGIT) 
                         | (char_('2') >> char_('2') >> char_('0', '4'));
    ttl                  = (POS_DIGIT >> repeat(0, 2)[abnf.DIGIT]) 
                         | char_('0');
    FQDN                 = repeat(4, inf)[alpha_numeric 
                                                 | char_('-') | char_('.')];
                        // fully qualified domain name as specified
                        // in RFC 1035 (and updates)
    IP4_address          = b1 >> repeat(3)[char_('.') >> decimal_uchar];
    b1                   = decimal_uchar;
                        // less than "224"

    /* The following is consistent with RFC 2373, Appendix B. */
    IP6_address          = hexpart >> -(char_(':') >> IP4_address);
    hexpart              = hexseq 
                         | (hexseq  >> char_(':') >> char_(':') >> -hexseq)
                         | (char_(':') >> char_(':') >> -hexseq);
    hexseq               = hex4 >> *(lit(':') >> hex4);
    hex4                 = repeat(1,4)[abnf.HEXDIG];

    /* generic for other address families */
    extn_addr            = non_ws_string;


    /* generic sub-rules: datatypes */
    text                 = byte_string;
    byte_string          = +(char_('\x01', '\x09')
                         | char_('\x0B', '\x0C') 
                         | char_('\x0E', '\xFF'));
    non_ws_string        = +(abnf.VCHAR | char_('\x80', '\xFF'));
    token_char           = char_('\x21') 
                         | char_('\x23', '\x27') 
                         | char_('\x2A', '\x2B')
                         | char_('\x2D', '\x2E')
                         | char_('\x30', '\x39')
                         | char_('\x41', '\x5A')
                         | char_('\x5E', '\x7E');
    token                = +(token_char);
    email_safe           = char_('\x01', '\x09') 
                         | char_('\x0B', '\x0C')
                         | char_('\x0E', '\x27')
                         | char_('\x2A', '\x3B')
                         | char_('\x3D')
                         | char_('\x3F', '\xFF');
    integer              = POS_DIGIT >> *abnf.DIGIT;

    /* generic sub-rules: primitives */
    alpha_numeric        = abnf.ALPHA | abnf.DIGIT;
    POS_DIGIT            = char_('\x31', '\x39');
    decimal_uchar        = abnf.DIGIT
                         | (POS_DIGIT >> abnf.DIGIT)
                         | (char_('1') >> repeat(2)[abnf.DIGIT]) 
                         | (char_('2') >> char_('0', '4') >> abnf.DIGIT) 
                         | (char_('2') >> char_('5') >> char_('0', '4'));

    /* naming */
    session_description.name("session-description");
    proto_version.name("proto-version");
    origin_field.name("origin-field");
    session_name_field.name("session-name-field");
    information_field.name("information-field");
    uri_field.name("uri-field");
    email_fields.name("email-fields");
    phone_fields.name("phone-fields");
    connection_field.name("connection_field");
    bandwidth_fields.name("bandwidth_fields");
    time_fields.name("time_fields");
    repeat_fields.name("repeat_fields");
    zone_adjustments.name("zone_adjustments");
    key_field.name("key_field");
    attribute_fields.name("attribute_fields");
    media_descriptions.name("media_descriptions");
    media_field.name("media_field");
    username.name("username");
    sess_id.name("sess_id");
    sess_version.name("sess-version");
    nettype.name("nettype");
    addrtype.name("addrtype");
    uri.name("uri");
    email_address.name("email-address");
    address_and_comment.name("address-and-comment");
    dispname_and_address.name("dispname-and-address");
    phone_number.name("phone-number");
    phone.name("phone");
    connection_address.name("connection_address");
    bwtype.name("bwtype");
    bandwidth.name("bandwidth");
    start_time.name("bandwidth");
    stop_time.name("stop_time");
    time.name("time");
    repeat_interval.name("repeat_interval");
    typed_time.name("typed_time");
    fixed_len_time_unit.name("typed_time");
    key_type.name("key_type");
    base64_char.name("base64_char");
    base64_unit.name("base64_unit");
    base64_pad.name("base64_pad");
    base64.name("base64");
    attribute.name("attribute");
    att_field.name("att_field");
    att_value.name("att_value");
    media.name("media");
    fmt.name("media");
    proto.name("proto");
    port.name("port");
    unicast_address.name("unicast_address");
    multicast_address.name("multicast_address");
    IP4_multicast.name("IP4_multicast");
    m1.name("m1");
    IP6_multicast.name("IP6_multicast");
    bm.name("bm");
    ttl.name("ttl");
    FQDN.name("FQDN");
    IP4_address.name("IP4_address");
    b1.name("b1");
    IP6_address.name("IP6_address");
    hexpart.name("hexpart");
    hexseq.name("hexseq");
    hex4.name("hex4");
    extn_addr.name("extn_addr");
    text.name("text");
    byte_string.name("byte_string");
    non_ws_string.name("non_ws_string");
    token_char.name("token_char");
    token.name("token");
    email_safe.name("email_safe");
    integer.name("integer");
    alpha_numeric.name("alpha_numeric");
    POS_DIGIT.name("POS_DIGIT");
    decimal_uchar.name("decimal_uchar");
}