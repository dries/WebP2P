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
 * Filename:    URIReferenceGrammar.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Implementation for the RFC 3986 uri-reference grammar rules.
 * See also:    http://tools.ietf.org/html/rfc3986#section-4.1
**/

/* WebP2P includes */
#include "URIReferenceGrammar.hpp"

URIReferenceGrammar::URIReferenceGrammar() :
    URIReferenceGrammar::base_type(uri_reference) {
    using           boost::spirit::qi::iso8859_1::string;
    using           boost::spirit::qi::iso8859_1::char_;
    using           boost::spirit::qi::repeat;
    using           boost::spirit::qi::_val;
    using           boost::spirit::qi::_1;

    /* URI: from RFC 3986 Appendix A */
    uri           %= scheme >> string(":") 
                  >> hier_part
                  >> -(string("?") >> query)
                  >> -(string("#") >> fragment);

    hier_part     %= (string("//") >> authority >> path_abempty)
                   | path_absolute
                   | path_rootless
                   | path_empty;

    uri_reference %= uri | relative_ref;

    absolute_uri  %= scheme >> string(":") 
                  >> hier_part
                  >> -(string("?") >> query);

    relative_ref  %= relative_part
                  >> -(string("?") >> query)
                  >> -(string("#") >> fragment);

    relative_part %= (string("/") >> string("/") >> authority >> path_abempty)
                   | path_absolute
                   | path_noscheme
                   | path_empty;

    scheme        %= abnf.ALPHA 
                  >> *(abnf.ALPHA | abnf.DIGIT 
                                  | string("+") | string("-") | string("."));

    authority     %= -(userinfo >> string("@")) 
                  >> host 
                  >> -(string(":") >> port);
    userinfo      %= *(unreserved | pct_encoded | sub_delims | string(":"));
    host          %= ip_literal | ipv4address | reg_name;
    port          %= *abnf.DIGIT[_val += _1];
        
    ip_literal    %= string("[") >> (ipv6address | ipvfuture) >> string("]");
        
    ipvfuture     %= string("v") >> +abnf.HEXDIG >> string(".") 
                  >> +(unreserved | sub_delims | string(":"));

    ipv6address   %= i1 | i2 | i3 | i4 | i5 | i6 | i7 | i8 | i9;
    i1            %=                             
                                     repeat(6)[h16 >> string(":")] >> ls32;
    i2            %=                 string("::")
                                  >> repeat(5)[h16 >> string(":")] >> ls32;
    i3            %= -(      h16) >> string("::")
                                  >> repeat(4)[h16 >> string(":")] >> ls32;
    i4            %= -(r1 >> h16) >> string("::")
                                  >> repeat(3)[h16 >> string(":")] >> ls32;
    i5            %= -(r2 >> h16) >> string("::")
                                  >> repeat(2)[h16 >> string(":")] >> ls32;
    i6            %= -(r3 >> h16) >> string("::")
                                  >>           h16 >> string(":")  >> ls32;
    i7            %= -(r4 >> h16) >> string("::")
                                  >>                                  ls32;
    i8            %= -(r5 >> h16) >> string("::")
                                  >>                                  h16 ;
    i9            %= -(r6 >> h16) >> string("::")
                                                                          ;
    r1            %= repeat(0, 1)[h16 >> string(":")];
    r2            %= repeat(0, 2)[h16 >> string(":")];
    r3            %= repeat(0, 3)[h16 >> string(":")];
    r4            %= repeat(0, 4)[h16 >> string(":")];
    r5            %= repeat(0, 5)[h16 >> string(":")];
    r6            %= repeat(0, 6)[h16 >> string(":")];

    h16           %= repeat(1, 4)[abnf.HEXDIG];
    ls32          %= (h16 >> string(":") >> h16) | ipv4address;

    ipv4address   %= dec_octet >> string(".") 
                  >> dec_octet >> string(".") 
                  >> dec_octet >> string(".") 
                  >> dec_octet;

    dec_octet     %= (char_('\x30', '\x39'))  // 0-9
                   | (char_('\x31', '\x39') 
                   >> char_('\x30', '\x39'))  // 10-99
                   | (char_('\x31')
                   >> char_('\x30', '\x39')
                   >> char_('\x30', '\x39'))  // 100-199
                   | (char_('\x32')
                   >> char_('\x30', '\x34')
                   >> char_('\x30', '\x39'))  // 200-249
                   | (char_('\x32')         
                   >> char_('\x35')
                   >> char_('\x30', '\x35')); // 250-255

    reg_name      %= *(unreserved | pct_encoded | sub_delims);

    path          %= path_abempty    // begins with "/" or is empty
                   | path_absolute   // begins with "/" but not "//"
                   | path_noscheme   // begins with a non-colon segment
                   | path_rootless   // begins with a segment
                   | path_empty;     // zero characters

    path_abempty  %=                                *(string("/") >> segment);
    path_absolute %= string("/") >> -(segment_nz >> *(string("/") >> segment));
    path_noscheme %= segment_nz_nc               >> *(string("/") >> segment);
    path_rootless %= segment_nz                  >> *(string("/") >> segment);
    path_empty    %= repeat(0)[pchar];

    segment       %= *pchar;
    segment_nz    %= +pchar;
    segment_nz_nc %= +(unreserved | pct_encoded | sub_delims | string("@"));
                  // non-zero-length segment without any colon ":"

    pchar         %= unreserved 
                   | pct_encoded
                   | sub_delims
                   | string(":")
                   | string("@");

    query         %= *(pchar | string("/") | string("?"));

    fragment      %= *(pchar | string("/") | string("?"));

    pct_encoded   %= string("%") >> abnf.HEXDIG >> abnf.HEXDIG;

    unreserved    %= abnf.ALPHA | abnf.DIGIT | string("-") 
                   | string(".") | string("_") | string("~");
    reserved      %= gen_delims | sub_delims;
    gen_delims    %= string(":") | string("/") | string("?") | string("#") 
                   | string("[") | string("]") | string("@");
    sub_delims    %= string("!") | string("$") | string("&") | string("\"")
                   | string("(") | string(")") | string("*") | string("+")
                   | string(",") | string(";") | string("=");
}