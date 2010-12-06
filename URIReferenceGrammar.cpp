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
 */

/* WebP2P includes */
#include "URIReferenceGrammar.hpp"

URIReferenceGrammar::URIReferenceGrammar() :
    URIReferenceGrammar::base_type(uri_reference) {
    using           boost::spirit::qi::ascii::char_;
    using namespace boost::spirit::qi::ascii;
    using namespace boost::spirit::qi;
    using namespace boost::spirit;

    /* URI: from RFC 3986 Appendix A */
    uri           = scheme >> char_(':') 
                 >> hier_part
                 >> -(char_('?') >> query)
                 >> -(char_('#') >> fragment);
        
    hier_part     = (char_('/') >> char_('/') >> authority >> path_abempty)
                  | path_absolute
                  | path_rootless
                  | path_empty;
        
    uri_reference = uri | relative_ref;
        
    absolute_uri  = scheme >> char_(':') 
                 >> hier_part
                 >> -(char_('?') >> query);
        
    relative_ref  = relative_part
                 >> -(char_('?') >> query)
                 >> -(char_('#') >> fragment);
        
    relative_part = (char_('/') >> char_('/') >> authority >> path_abempty)
                  | path_absolute
                  | path_noscheme
                  | path_empty;
        
    scheme        = abnf.ALPHA 
                 >> *(abnf.ALPHA | abnf.DIGIT 
                                 | char_('+') | char_('-') | char_('.'));

    authority     = -(userinfo >> char_('@')) 
                 >> host 
                 >> -(char_(':') >> port);
    userinfo      = *(unreserved | pct_encoded | sub_delims | char_(':'));
    host          = ip_literal | ipv4address | reg_name;
    port          = *abnf.DIGIT;
        
    ip_literal    = char_('[') >> (ipv6address | ipvfuture) >> char_(']');
        
    ipvfuture     = char_('v') >> +abnf.HEXDIG >> char_('.') 
                 >> +(unreserved | sub_delims | char_(':'));
        
    ipv6address   = (                                        
                                    repeat(6)[h16 >> char_(':')] >> ls32)
                  | (                                             char_(':')
                   >> char_(':') >> repeat(5)[h16 >> char_(':')] >> ls32)
                  | (-(                                   h16) >> char_(':')
                   >> char_(':') >> repeat(4)[h16 >> char_(':')] >> ls32)
                  | (-(repeat(0, 1)[h16 >> char_(':')] >> h16) >> char_(':')
                   >> char_(':') >> repeat(3)[h16 >> char_(':')] >> ls32)
                  | (-(repeat(0, 2)[h16 >> char_(':')] >> h16) >> char_(':')
                   >> char_(':') >> repeat(2)[h16 >> char_(':')] >> ls32)
                  | (-(repeat(0, 3)[h16 >> char_(':')] >> h16) >> char_(':')
                   >> char_(':') >>           h16 >> char_(':')  >> ls32)
                  | (-(repeat(0, 4)[h16 >> char_(':')] >> h16) >> char_(':')
                   >> char_(':') >>                                 ls32)
                  | (-(repeat(0, 5)[h16 >> char_(':')] >> h16) >> char_(':')
                   >> char_(':') >>                                 h16 )
                  | (-(repeat(0, 6)[h16 >> char_(':')] >> h16) >> char_(':')
                   >> char_(':')                                        );
        
    h16           = repeat(1, 4)[abnf.HEXDIG];
    ls32          = (h16 >> char_(':') >> h16) | ipv4address;
     
    ipv4address   = dec_octet >> char_('.') 
                 >> dec_octet >> char_('.') 
                 >> dec_octet >> char_('.') 
                 >> dec_octet;
        
    dec_octet     = (char_('\x30', '\x39'))  // 0-9
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
        
    reg_name      = *(unreserved | pct_encoded | sub_delims);
        
    path          = path_abempty    // begins with "/" or is empty
                  | path_absolute   // begins with "/" but not "//"
                  | path_noscheme   // begins with a non-colon segment
                  | path_rootless   // begins with a segment
                  | path_empty;     // zero characters
        
    path_abempty  =                               *(char_('/') >> segment);
    path_absolute = char_('/') >> -(segment_nz >> *(char_('/') >> segment));
    path_noscheme = segment_nz_nc              >> *(char_('/') >> segment);
    path_rootless = segment_nz                 >> *(char_('/') >> segment);
    path_empty    = repeat(0)[pchar];
        
    segment       = *pchar;
    segment_nz    = +pchar;
    segment_nz_nc = +(unreserved | pct_encoded | sub_delims | char_('@'));
                 // non-zero-length segment without any colon ":"
        
    pchar         = unreserved 
                  | pct_encoded
                  | sub_delims
                  | char_(':')
                  | char_('@');
        
    query         = *(pchar | char_('/') | char_('?'));
        
    fragment      = *(pchar | char_('/') | char_('?'));
        
    pct_encoded   = char_('%') >> abnf.HEXDIG >> abnf.HEXDIG;
        
    unreserved    = abnf.ALPHA | abnf.DIGIT | char_('-') 
                  | char_('.') | char_('_') | char_('~');
    reserved      = gen_delims | sub_delims;
    gen_delims    = char_(':') | char_('/') | char_('?') | char_('#') 
	               | char_('[') | char_(']') | char_('@');
    sub_delims    = char_('!') | char_('$') | char_('&') | char_('\'')
                   | char_('(') | char_(')') | char_('*') | char_('+')
                   | char_(',') | char_(';') | char_('=');
}