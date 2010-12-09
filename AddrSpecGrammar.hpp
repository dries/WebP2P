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
 * Filename:    AddrSpecGrammar.hpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Definition for the RFC 2822 addr-spec grammar rules.
 * See also:    http://tools.ietf.org/html/rfc2822#section-3
**/

#pragma once

/* WebP2P includes */
#include "ABNFCoreGrammar.hpp"

struct AddrSpecGrammar :
    boost::spirit::qi::grammar<std::string::const_iterator, std::string()> {
    /* external grammars */
    ABNFCoreGrammar abnf;

    /* rules for this grammar */
    boost::spirit::qi::rule<std::string::const_iterator>
        obs_qp, obs_text, obs_char, obs_FWS, obs_domain, obs_local_part, 
        NO_WS_CTL, text, specials, quoted_pair, FWS, ctext, ccontent, comment,
        CFWS, atext, atom, dot_atom_text, dot_atom, qtext, qcontent,
        quoted_string, word, dtext, dcontent, domain_literal, domain,
        local_part;
    
    /* start symbol for this grammar */
    boost::spirit::qi::rule<std::string::const_iterator, std::string()>
        addr_spec;
    
    AddrSpecGrammar();
};
