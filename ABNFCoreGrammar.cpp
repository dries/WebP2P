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
 */

/* WebP2P includes */
#include "ABNFCoreGrammar.hpp"

ABNFCoreGrammar::ABNFCoreGrammar() {
    using           boost::spirit::qi::ascii::char_;
    using namespace boost::spirit::qi::ascii;
    using namespace boost::spirit::qi;
    using namespace boost::spirit;

    /* Core rules: from RFC 4234 */
    SP     = char_('\x20');
    CR     = char_('\x0D');
    LF     = char_('\x0A');
    HTAB   = char_('\x09');
    DQUOTE = char_('\x22');
    CHAR   = char_('\x01', '\x7F');
    VCHAR  = char_('\x21', '\x7E');
    DIGIT  = char_('\x30', '\x39');
    BIT    = char_('\x30', '\x31');
    OCTET  = char_('\x00', '\xFF');
    CTL    = char_('\x00', '\x1F') | char_('\x7F');
    ALPHA  = char_('\x41', '\x5A') | char_('\x61', '\x7A');
    HEXDIG = DIGIT | char_('A', 'F');
    WSP    = SP | HTAB;
    CRLF   = CR > LF;
    LWSP   = *(WSP | (CRLF > WSP));
}