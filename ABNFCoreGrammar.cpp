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
 * Filename:    ABNFCoreGrammar.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Implementation for the RFC 4234 ABNF core grammar rules.
 * See also:    http://tools.ietf.org/html/rfc4234#appendix-B.1
**/

/* WebP2P includes */
#include "ABNFCoreGrammar.hpp"

ABNFCoreGrammar::ABNFCoreGrammar() {
    using           boost::spirit::qi::iso8859_1::char_;
    using namespace boost::spirit::qi::iso8859_1;
    using namespace boost::spirit::qi;
    using namespace boost::spirit;

    /* Core rules: from RFC 4234 */
    SP     = char_('\x20')[_val += _1];
    CR     = char_('\x0D')[_val += _1];
    LF     = char_('\x0A')[_val += _1];
    HTAB   = char_('\x09')[_val += _1];
    DQUOTE = char_('\x22')[_val += _1];
    CHAR   = char_('\x01', '\x7F')[_val += _1];
    VCHAR  = char_('\x21', '\x7E')[_val += _1];
    DIGIT  = char_('\x30', '\x39')[_val += _1];
    BIT    = char_('\x30', '\x31')[_val += _1];
    OCTET  = char_[_val += _1];
    CTL    = (char_('\x00', '\x1F') | char_('\x7F'))[_val += _1];
    ALPHA  = (char_('\x41', '\x5A') | char_('\x61', '\x7A'))[_val += _1];
    HEXDIG = DIGIT[_val += _1]
           | char_('A', 'F')[_val += _1];
    WSP   %= (SP | HTAB);
    CRLF  %= (CR > LF);
    LWSP   = *(WSP[_val += _1] | (CRLF > WSP)[_val += _1]);
}