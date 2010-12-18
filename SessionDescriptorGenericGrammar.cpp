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
 * Filename:    SessionDescriptorGenericGrammar.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Implementation for the RFC 4566 SDP grammar rules.
 * See:         http://tools.ietf.org/html/rfc4566
**/

/* Boost includes */
#include <boost/spirit/home/phoenix.hpp>

/* WebP2P includes */
#include "SessionDescriptorGenericGrammar.hpp"

SessionDescriptorGenericGrammar::SessionDescriptorGenericGrammar() {
    using           boost::spirit::qi::iso8859_1::char_;
    using           boost::spirit::qi::iso8859_1::string;
    using           boost::spirit::qi::iso8859_1::digit;
    using namespace boost::spirit::qi::iso8859_1;
    using namespace boost::spirit::qi;

    /* generic sub-rules: addressing */
    unicast_address     %= IP4_address
                         | IP6_address
                         | FQDN
                         | extn_addr;
    multicast_address   %= IP4_multicast
                         | IP6_multicast
                         | FQDN
                         | extn_addr;
    IP4_multicast       %= bm 
                        >> char_('.') >> decimal_uchar
                        >> char_('.') >> decimal_uchar
                        >> char_('.') >> decimal_uchar
                        >> char_('/') >> ttl
                        >> -(char_('/') >> integer);
                        // IPv4 multicast addresses may be in the
                        // range 224.0.0.0 to 239.255.255.255
    bm1                 %= string("22") >> char_('4', '9');
    bm2                 %= string("23") >> char_('0', '9');
    bm                  %= bm1 | bm2;
    IP6_multicast       %= hexpart >> -(char_('/') >> integer);
                        // IPv6 address starting  with FF
    ttl                  = +digit;
    FQDN                 = repeat(4, inf)[
                                alpha_numeric[_val += _1]
                              | string("-")[_val += _1]
                              | string(".")[_val += _1]];
                        // fully qualified domain name as specified
                        // in RFC 1035 (and updates)
    IP4_address         %= +digit
                        >> string(".") >> +digit
                        >> string(".") >> +digit
                        >> string(".") >> +digit;
    bu1                 %= char_('2') >> char_('2')      >> char_('0', '3');
    bu2                 %= char_('2') >> char_('0', '1') >> char_('0', '9');
    bu3                 %= char_('1') >> char_('0', '9') >> char_('0', '9');
    bu4                 %=               char_('0', '9') >> char_('0', '9');
    bu5                 %=                                  char_('0', '9');
    bu                  %= bu1 | bu2 | bu3 | bu4 | bu5;
                        // less than "224"

    /* The following is consistent with RFC 2373, Appendix B. */
    IP6_address         %= hexpart >> -(string(":") >> IP4_address);
    hexpart             %= (hexseq  >> string("::") >> -hexseq)
                         | (           string("::") >> -hexseq)
                         | (                            hexseq);
    hexseq              %= hex4 >> *(lit(":") >> hex4);
    hex4                %= repeat(1,4)[abnf.HEXDIG];

    /* generic for other address families */
    extn_addr           %= non_ws_string.alias();

    /* generic sub-rules: datatypes */
    text                %= byte_string.alias();
                        // default is to interpret this as UTF8 text.
                        // ISO 8859-1 requires "a=charset:ISO-8859-1"
                        // session-level attribute to be used
    byte_string         %= +(char_('\x00', '\x09')
                         |   char_('\x0B', '\x0C')
                         |   char_('\x0E', '\xFF'));
    non_ws_string       %= +(char_('\x21', '\x7E')
                         |   char_('\x80', '\xFF'));
                        // string of visible characters
    token               %= +(char_('\x21') 
                         |   char_('\x23', '\x27') 
                         |   char_('\x2A', '\x2B')
                         |   char_('\x2D', '\x2E')
                         |   char_('\x30', '\x39')
                         |   char_('\x41', '\x5A')
                         |   char_('\x5E', '\x7E'));
    email_safe           = !(char_('\x00')
                         |   char_('\x0A')
                         |   char_('\x0D')
                         |   char_('\x28')
                         |   char_('\x29')
                         |   char_('\x3C')
                         |   char_('\x3E'))
                        >> char_[_val += _1];
                        // any byte except NUL, CR, LF, or the quoting
                        // characters ()<>
    integer             %= char_('1', '9') 
                        >> *char_('0', '9');

    /* generic sub-rules: primitives */
    alpha_numeric        = char_('\x41', '\x5A')[_val += _1]
                         | char_('\x61', '\x7A')[_val += _1]
                         | char_('0', '9')      [_val += _1];
    POS_DIGIT            = char_('1', '9')      [_val += _1];
    du1                 %= char_('2') >> char_('5')      >> char_('0', '5');
    du2                 %= char_('2') >> char_('0', '4') >> char_('0', '9');
    du3                 %= char_('1') >> char_('0', '9') >> char_('0', '9');
    du4                 %=               char_('0', '9') >> char_('0', '9');
    du5                 %=                                  char_('0', '9');
    decimal_uchar       %= du1 | du2 | du3 | du4 | du5;

    unicast_address.name("unicast-address");
    multicast_address.name("multicast-address");
    IP4_multicast.name("IP4-multicast");
    IP6_multicast.name("IP6-multicast");
    bm.name("bm");
    ttl.name("ttl");
    FQDN.name("FQDN");
    IP4_address.name("IP4-address");
    bu.name("bu");
    IP6_address.name("IP6-address");
    hexpart.name("hexpart");
    hexseq.name("hexseq");
    hex4.name("hex4");
    extn_addr.name("extn-addr");
    text.name("text");
    byte_string.name("byte-string");
    non_ws_string.name("non-ws-string");
    email_safe.name("email-safe");
    integer.name("integer");
    alpha_numeric.name("alpha-numeric");
    POS_DIGIT.name("POS-DIGIT");
    decimal_uchar.name("decimal-uchar");
}