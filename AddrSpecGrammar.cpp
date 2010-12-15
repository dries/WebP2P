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
 * Description: Implementation for the RFC 2822 addr-spec grammar rules.
 * See also:    http://tools.ietf.org/html/rfc2822#section-3
**/

/* WebP2P includes */
#include "AddrSpecGrammar.hpp"

AddrSpecGrammar::AddrSpecGrammar() :
    AddrSpecGrammar::base_type(addr_spec) {
    using           boost::spirit::qi::iso8859_1::char_;
    using namespace boost::spirit::qi::iso8859_1;
    using namespace boost::spirit::qi;
    using namespace boost::spirit;

    // obsolete grammar which we still have to accept but should not generate
    obs_qp          %= char_('\\') >> char_('\x00', '\x7F');
    obs_text         = *abnf.LF[_val += _1] 
                    >> *abnf.CR[_val += _1]
                    >> *(obs_char[_val += _1]
                      >> *abnf.LF[_val += _1]
                      >> *abnf.CR[_val += _1]);
    obs_char         = (char_('\x00', '\x09')  // %d0-127 except CR and LF
                     |  char_('\x0B', '\x0C')  //
                     |  char_('\x0E', '\x7F')) //
                          [_val += _1];
    obs_FWS          = +abnf.WSP[_val += _1]
                    >> *(abnf.CRLF[_val += _1]
                      >> +abnf.WSP[_val += _1]);
    obs_domain      %= atom >> *(char_('.') >> atom);
    obs_local_part  %= word >> *(char_('.') >> word);

    /* addr-spec: from RFC 2822 */
    NO_WS_CTL       = (char_('\x01', '\x08')
                    |  char_('\x0B', '\x0C')
                    |  char_('\x0E', '\x1F')
                    |  char_('\x7F'))
                         [_val += _1];
    text            = (char_('\x01', '\x09')
                    |  char_('\x0B', '\x0C') 
                    |  char_('\x0E', '\x1F'))
                         [_val += _1];/* | obs_text[_val += _1]*/;
    specials        = (char_('(') | char_(')' ) | char_('<') | char_('>')
                    |  char_('[') | char_(']' ) | char_(':') | char_(';')
                    |  char_('@') | char_('\\') | char_(',') | char_('.') 
                    |  char_('"'))
                         [_val += _1];

    quoted_pair    %= (char_('\\') >> text)/* | obs_qp*/;

    FWS             = (-(*abnf.WSP[_val += _1] 
                      >> abnf.CRLF[_val += _1])
                    >> +abnf.WSP[_val += _1])/* | obs_FWS[_val += _1]*/;
    ctext           = NO_WS_CTL[_val += _1]
                    | char_('\x21', '\x27')[_val += _1]
                    | char_('\x2A', '\x5B')[_val += _1]
                    | char_('\x5D', '\x7E')[_val += _1];
    ccontent       %= ctext | quoted_pair | comment;
    comment         = char_('(')[_val += _1]
                   >> *(-FWS[_val += _1] >> ccontent[_val += _1])
                   >> -FWS[_val += _1]
                   >> char_(')')[_val += _1];
    CFWS            = *(-FWS[_val += _1] >> comment[_val += _1])
                      >> ((-FWS[_val += _1] >> comment[_val += _1]) 
                         | FWS[_val += _1]);

    atext           = (abnf.ALPHA | abnf.DIGIT)[_val += _1]
                    | (char_('!') | char_('#') | char_('$') | char_('%')
                      | char_('&') | char_('\'') | char_('*') | char_('+')
                      | char_('-') | char_('/') | char_('=') | char_('?')
                      | char_('^') | char_('_') | char_('`') | char_('{')
                      | char_('|') | char_('}') | char_('~'))[_val += _1];
    atom            = -CFWS[_val += _1] 
                   >> +atext[_val += _1]
                   >> -CFWS[_val += _1];
    dot_atom_text   = +atext[_val += _1]
                   >> *(char_('.')[_val += _1] >> +atext[_val += _1]);
    dot_atom       %= -CFWS >> dot_atom_text >> -CFWS;

    qtext           = NO_WS_CTL[_val += _1]
                    | char_('\x21')[_val += _1]
                    | char_('\x23', '\x5B')[_val += _1]
                    | char_('\x5D', '\x7E')[_val += _1];
    qcontent       %= qtext | quoted_pair;
    quoted_string   = -CFWS[_val += _1]
                   >> abnf.DQUOTE[_val += _1]
                   >> *(-FWS[_val += _1] >> qcontent[_val += _1]) 
                   >> -FWS[_val += _1]
                   >> abnf.DQUOTE[_val += _1]
                   >> -CFWS[_val += _1];

    word           %= atom | quoted_string;

    dtext           = NO_WS_CTL[_val += _1]
                    | char_('\x21', '\x5A')[_val += _1]
                    | char_('\x5E', '\x7E')[_val += _1];
    dcontent       %= dtext | quoted_pair;
    domain_literal  = -CFWS 
                   >> char_('[')[_val += _1]
                   >> *(-FWS[_val += _1] >> dcontent[_val += _1])
                   >> -FWS[_val += _1]
                   >> char_(']')[_val += _1]
                   >> -CFWS[_val += _1];

    domain         %= dot_atom | domain_literal/* | obs_domain*/;
    local_part     %= dot_atom | quoted_string/* | obs_local_part*/;
    addr_spec      %= local_part >> lit("@") >> domain;
}