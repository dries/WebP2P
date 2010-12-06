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
 * Description: Definition for the RFC 3986 uri-reference grammar rules.
 * See also:    http://tools.ietf.org/html/rfc3986#section-4.1
 */

#pragma once

/* WebP2P includes */
#include "ABNFCoreGrammar.hpp"

struct URIReferenceGrammar :
    boost::spirit::qi::grammar<std::string::const_iterator> {
    /* external grammars */
    ABNFCoreGrammar abnf;

    /* rules for this grammar */
    boost::spirit::qi::rule<std::string::const_iterator>
        uri, hier_part, absolute_uri, relative_ref, relative_part, scheme,
        authority, userinfo, host, port, ip_literal, ipvfuture, ipv6address,
        h16, ls32, ipv4address, dec_octet, reg_name, path, path_abempty,
        path_absolute, path_noscheme, path_rootless, path_empty, segment,
        segment_nz, segment_nz_nc, pchar, query, fragment, pct_encoded,
        unreserved, reserved, gen_delims, sub_delims;
        
    /* start symbol for this grammar */
    boost::spirit::qi::rule<std::string::const_iterator> uri_reference;

	/* Grammar constructor */
	URIReferenceGrammar();
};