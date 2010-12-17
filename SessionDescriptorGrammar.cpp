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
    /* UTF-8 not supported in spirit::qi, fallback to iso-8859-1 so at least
     * the entire byte range is accepted.
    **/
    using           boost::spirit::qi::iso8859_1::string;
    using           boost::spirit::qi::iso8859_1::char_;
    using           boost::spirit::qi::iso8859_1::space;
    using namespace boost::spirit::qi::iso8859_1;
    using namespace boost::spirit::qi;
    using           boost::spirit::eol;
    using namespace boost::phoenix;
    using namespace boost::phoenix::local_names;

    /* Define error handler */
    on_error<fail>
    (
        session_description,
        let(_s = construct<std::string>())
        [
            _s += val("Error! Expecting "),
            //_s += _4, // TODO: figure out why this doesn't work
            _s += val(" here: \""),
            _s += construct<std::string>(_3, _2),
            _s += val("\"\n"),
            throw_(construct<ParseException>(_s))
        ]
    );
    
    /* The start symbol */
    session_description  %= proto_version
                         >> origin_field
                         >> session_name_field
//                         >> information_field
//                         >> uri_field
//                         >> email_fields
//                         >> phone_fields
//                         >> -connection_field
//                         >> bandwidth_fields
                         >> time_fields
//                         >> key_field
                         >> attribute_fields
                         >> media_descriptions;

    /* SDP fields */
    proto_version        %=   lit("v=")
                         >> ulong_long          >> eol;
    origin_field         %=   lit("o=")
                         >> username            >> lit(' ')
                         >> sess_id             >> lit(' ')
                         >> sess_version        >> lit(' ')
                         >> nettype             >> lit(' ')
                         >> addrtype            >> lit(' ')
                         >> sdp.unicast_address >> eol;
    session_name_field   %=   lit("s=")
                         >> sdp.text            >> eol;
    information_field    %= -(lit("i=")
                         >> sdp.text            >> eol);
    uri_field            %= -(lit("u=")
                         >> uri                 >> eol);
    email_fields         %= *(lit("e=")
                         >> email_address       >> eol);
    phone_fields         %= *(lit("p=")
                         >> phone_number        >> eol);
    connection_field     %=   lit("c=")
                         >> nettype             >> lit(' ')
                         >> addrtype            >> lit(' ')
                         >> connection_address  >> eol;
                         // a connection field must be present in every media
                         // description or at the session-level
    bandwidth_fields     %= *(lit("b=")
                         >> bwtype              >> lit(':') 
                         >> bandwidth           >> eol);
    time_fields          %= +(lit("t=")
                         >> start_time          >> lit(' ')
                         >> stop_time 
/*                         >> *(eol >> repeat_fields)*/
                         >> eol)
/*                         >> -(zone_adjustments  >> eol)*/;
    repeat_fields        %=   lit("r=")
                         >> repeat_interval     >> lit(' ')
                         >> typed_time          >> +(lit(' ') >> typed_time);
    zone_adjustments     %=   lit("z=")
                         >> ulong_long          >> lit(' ') 
                         >> long_long           %  lit(' ');
                         // TODO: parse typed-time
    key_field            %= -(lit("k=")
                         >> key_type            >> eol);
    attribute_fields     %= *(lit("a=")
                         >> attribute           >> eol);
    media_descriptions   %= *(media_field
//                         >> information_field
                         >> *connection_field
//                         >> bandwidth_fields
//                         >> key_field
                         >> attribute_fields);
    media_field          %=   lit("m=")
                         >> media >> lit(' ') 
                         >> port /*>> -(lit('/') >> sdp.integer)*/ >> lit(' ') 
                         >> proto >> +(lit(' ') >> fmt) >> eol;                           

    /* sub-rules of 'o=' */
    username             %= sdp.non_ws_string.alias();
                         // pretty wide definition, but doesn't include space
    sess_id              %= ulong_long;
                         // should be unique for this username/host
    sess_version         %= ulong_long;
    nettype              %= sdp.token.alias();
                         // typically "IN"
    addrtype             %= sdp.token.alias();
                         // typically "IP4" or "IP6"

    /* sub-rules of 'u=' */
    uri                  %= URI_reference; // see RFC 3986

    /* sub-rules of 'e=', see RFC 2822 for definitions */
    email_address        %= dispname_and_address
                          | address_and_comment
                          | addr_spec;
    address_and_comment  %= addr_spec
                         >> +abnf.SP 
                         >> char_('(')
                         >> +sdp.email_safe
                         >> char_(')');
    dispname_and_address %= +sdp.email_safe
                         >> +abnf.SP
                         >> char_('<')
                         >> addr_spec
                         >> char_('>');

    /* sub-rules of 'p=' */
    phone_number         %= (phone >> *abnf.SP >> char_('(') 
                                               >> +sdp.email_safe >> char_(')')) 
                          | (+sdp.email_safe >> char_('<') 
                                                         >> phone >> char_('>'))
                          | phone;
    phone                %= -char_('+') >> abnf.DIGIT 
                                        >> +(abnf.SP | char_('-') | abnf.DIGIT);

    /* sub-rules of 'c=' */
    connection_address   %= sdp.unicast_address
                          | sdp.multicast_address;

    /* sub-rules of 'b=' */
    bwtype               %= sdp.token.alias();
    bandwidth            %= ulong_long;

    /* sub-rules of 't=' */
    /* Decimal representation of NTP time in seconds since 1900.  The
     * representation of NTP time is an unbounded length field containing at
     * least 10 digits.  Unlike the 64-bit representation used elsewhere, time
     * in SDP does not wrap in the year 2036. -- ANNOYING!
     */
    start_time           %= ulong_long;
    stop_time            %= ulong_long;
    time                 %= ulong_long;

    /* sub-rules of 'r=' and 'z=' */
    repeat_interval      %= ulong_long
                         >> -fixed_len_time_unit;
    typed_time           %= ulong_long
                         >> -fixed_len_time_unit;
    fixed_len_time_unit  %= char_('d')
                          | char_('h')
                          | char_('m')
                          | char_('s');

    /* sub-rules of 'k=' */
    key_type             %=  string("prompt") 
                          | (string("clear:") >> sdp.text)
                          | (string("base64:") >> base64)
                          | (string("uri:") >> uri);
    base64_char          %= abnf.ALPHA 
                          | abnf.DIGIT
                          | char_('+') 
                          | char_('/');
    base64_unit          %= repeat(4)[base64_char];
    base64_pad           %= repeat(2)[base64_char] >> repeat(2)[char_('=')] 
                          | repeat(3)[base64_char] >> char_('=');
    base64               %= *base64_unit >> -base64_pad;

    /* sub-rules of 'a=' */
    attribute            %= (att_field >> lit(':') >> att_value) | att_field;
    att_field            %= sdp.token.alias();
    att_value            %= sdp.byte_string.alias();

    /* sub-rules of 'm=' */
    media                %= sdp.token.alias();
                         // typically "audio", "video", "text", or "application"
    fmt                  %= sdp.token.alias();
                         // typically an RTP payload type for audio and video
                         // media
    proto                 = sdp.token[_val += _1]
                         >> *(char_('/')[_val += _1] >> sdp.token[_val += _1]); 
                         // typically "RTP/AVP" or "udp"
    port                 %= ushort_;


    /* naming */
    session_description.name("session-description");
    proto_version.name("proto-version");
    origin_field.name("origin-field");
    session_name_field.name("session-name-field");
    information_field.name("information-field");
    uri_field.name("uri-field");
    email_fields.name("email-fields");
    phone_fields.name("phone-fields");
    connection_field.name("connection-field");
    bandwidth_fields.name("bandwidth-fields");
    time_fields.name("time-fields");
    repeat_fields.name("repeat-fields");
    zone_adjustments.name("zone-adjustments");
    key_field.name("key-field");
    attribute_fields.name("attribute-fields");
    media_descriptions.name("media-descriptions");
    media_field.name("media-field");
    username.name("username");
    sess_id.name("sess-id");
    sess_version.name("sess-version");
    nettype.name("nettype");
    addrtype.name("addrtype");
    uri.name("uri");
    email_address.name("email-address");
    address_and_comment.name("address-and-comment");
    dispname_and_address.name("dispname-and-address");
    phone_number.name("phone-number");
    phone.name("phone");
    connection_address.name("connection-address");
    bwtype.name("bwtype");
    bandwidth.name("bandwidth");
    start_time.name("bandwidth");
    stop_time.name("stop-time");
    time.name("time");
    repeat_interval.name("repeat-interval");
    typed_time.name("typed-time");
    fixed_len_time_unit.name("typed-time");
    key_type.name("key-type");
    base64_char.name("base64-char");
    base64_unit.name("base64-unit");
    base64_pad.name("base64-pad");
    base64.name("base64");
    attribute.name("attribute");
    att_field.name("att-field");
    att_value.name("att-value");
    media.name("media");
    fmt.name("media");
    proto.name("proto");
    port.name("port");
}