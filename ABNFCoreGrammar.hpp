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
 * Filename:    ABNFCoreGrammar.hpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Definition for the RFC 4234 ABNF core grammar rules.
 * See also:    http://tools.ietf.org/html/rfc4234#appendix-B.1
 */

#pragma once

/* STL includes */
#include <string>

/* Boost includes */
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_repeat.hpp>

struct ABNFCoreGrammar {
    /* rules for this grammar */
    boost::spirit::qi::rule<std::string::const_iterator>
        CHAR, VCHAR, DIGIT, BIT, OCTET, CTL, ALPHA,
        SP, CR, LF, HTAB, DQUOTE, HEXDIG, WSP, CRLF, LWSP;

    ABNFCoreGrammar();
};
