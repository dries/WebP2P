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
 * Filename:    SessionDescriptorGenericGrammar.hpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Definition for the RFC 4566 SDP grammar rules.
 * See:         http://tools.ietf.org/html/rfc4566
**/

#pragma once

/* STL includes */
#include <string>

/* Boost includes */
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/spirit/include/qi_binary.hpp>

/* WebP2P includes */
#include "ABNFCoreGrammar.hpp"

struct SessionDescriptorGenericGrammar {
    /* external grammars */
    ABNFCoreGrammar                 abnf;

    /* rules for this grammar */
    boost::spirit::qi::rule<std::string::const_iterator, std::string()>
        unicast_address, multicast_address, IP4_multicast, m1, IP6_multicast,
        bm, ttl, FQDN, IP4_address, bu, IP6_address, hexpart, hexseq, hex4,
        extn_addr, text, byte_string, non_ws_string, token,
        email_safe, integer, alpha_numeric, POS_DIGIT, decimal_uchar,
        du1, du2, du3, du4, du5, bm1, bm2, bm3, bm4, bm5, bu1, bu2, bu3, bu4,
        bu5;

    SessionDescriptorGenericGrammar();
};
