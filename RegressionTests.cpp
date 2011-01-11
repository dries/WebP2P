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
 * Filename:    RegressionTests.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2010 Dries Staelens
 * Description: Implementation for the class that implements the javascript
 *              regression test interface.
**/

/* STL includes */
#include <vector>
#include <utility>

/* Boost includes */
#include <boost/thread/thread.hpp>
#include <boost/spirit/home/phoenix/container.hpp>
#include <boost/spirit/include/karma.hpp>

/* Firebreath includes */
#include "variant_list.h"

/* WebP2P includes */
#include "RegressionTests.hpp"

/* Test includes */
#include "AddrSpecGrammar.hpp"
#include "SessionDescriptorGrammar.hpp"
#include "URIReferenceGrammar.hpp"

enum TestResult { TEST_FAILED, TEST_SUCCEEDED };

struct GrammarTests {
    template<typename F> static void grammarTest(
        const std::string& n,
        const std::string& s,
        boost::spirit::qi::rule<std::string::const_iterator> r,
        F callback,
        bool shouldMatch = true) {
        using namespace boost::spirit::qi;
        
        std::string::const_iterator f = s.begin();
        std::string::const_iterator l = s.end();
        phrase_parse(f, l, r, !byte_);
        
        TestResult res;
        if(shouldMatch) {
            res = (f == l) ? TEST_SUCCEEDED : TEST_FAILED;
        } else {
            res = (f != l) ? TEST_SUCCEEDED : TEST_FAILED;
        }

        std::string testName = "Grammar test: " + n + " (" + s + ")";
        std::string testResult = (res == TEST_SUCCEEDED) ? "SUCCEEDED" : "FAILED";
        callback(testName, testResult);
    }
    template<typename F> static void runTests(F callback) {
        runAddrSpecTests(callback);
        runSessionDescriptorGenericGrammarTests(callback);
        runURIReferenceGrammarTests(callback);
        runSessionDescriptorGrammarTests(callback);
    }
    
    template<typename F> static void runAddrSpecTests(F callback) {
        AddrSpecGrammar asg;

        grammarTest("addr-spec", "dries@pandion.im", asg.addr_spec, callback, true);
        
        /* from: http://fightingforalostcause.net/misc/2006/compare-email-regex.php */
        grammarTest("addr-spec", "l3tt3rsAndNumb3rs@domain.com", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "has-dash@domain.com", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "hasApostrophe.o'leary@domain.org", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "uncommonTLD@domain.museum", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "uncommonTLD@domain.travel", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "uncommonTLD@domain.mobi", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "countryCodeTLD@domain.uk", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "countryCodeTLD@domain.rw", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "lettersInDomain@911.com", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "underscore_inLocal@domain.net", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "IPInsteadOfDomain@127.0.0.1", asg.addr_spec, callback, true);
//        grammarTest("addr-spec", "IPAndPort@127.0.0.1:25", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "subdomain@sub.domain.com", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "local@dash-inDomain.com", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "dot.inLocal@foo.com", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "a@singleLetterLocal.org", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "singleLetterDomain@x.org", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "&*=?^+{}'~@validCharsInLocal.net", asg.addr_spec, callback, true);
        grammarTest("addr-spec", "foor@bar.newTLD", asg.addr_spec, callback, true);

        grammarTest("!addr-spec", "missingDomain@.com", asg.addr_spec, callback, false);
        grammarTest("!addr-spec", "@missingLocal.org", asg.addr_spec, callback, false);
        grammarTest("!addr-spec", "missingatSign.net", asg.addr_spec, callback, false);
//        grammarTest("!addr-spec", "missingDot@com", asg.addr_spec, callback, false);
        grammarTest("!addr-spec", "two@@signs.com", asg.addr_spec, callback, false);
        grammarTest("!addr-spec", "colonButNoPort@127.0.0.1:", asg.addr_spec, callback, false);
//        grammarTest("!addr-spec", "", asg.addr_spec, callback, false);
//        grammarTest("!addr-spec", "someone-else@127.0.0.1.26", asg.addr_spec, callback, false);
        grammarTest("!addr-spec", ".localStartsWithDot@domain.com", asg.addr_spec, callback, false);
        grammarTest("!addr-spec", "localEndsWithDot.@domain.com", asg.addr_spec, callback, false);
        grammarTest("!addr-spec", "two..consecutiveDots@domain.com", asg.addr_spec, callback, false);
//        grammarTest("!addr-spec", "domainStartsWithDash@-domain.com", asg.addr_spec, callback, false);
//        grammarTest("!addr-spec", "domainEndsWithDash@domain-.com", asg.addr_spec, callback, false);
//        grammarTest("!addr-spec", "numbersInTLD@domain.c0m", asg.addr_spec, callback, false);
        grammarTest("!addr-spec", "missingTLD@domain.", asg.addr_spec, callback, false);
        grammarTest("!addr-spec", "! \"#$%(),/;<>[]`|@invalidCharsInLocal.org", asg.addr_spec, callback, false);
        grammarTest("!addr-spec", "invalidCharsInDomain@! \"#$%(),/;<>_[]`|.org", asg.addr_spec, callback, false);
//        grammarTest("!addr-spec", "local@SecondLevelDomainNamesAreInvalidIfTheyAreLongerThan64Charactersss.org", asg.addr_spec, callback, false);
    }
    
    template<typename F> static void runURIReferenceGrammarTests(F callback) {
        URIReferenceGrammar urg;
        
        grammarTest("URI-reference", "http://www.google.com/search?q=good+url+regex&rls=com.microsoft:*&ie=UTF-8&oe=UTF-8&startIndex=&startPage=1", urg.uri_reference, callback, true);
        grammarTest("!URI-reference", "http://a:b:c?q/∫", urg.uri_reference, callback, false);
    }
    
    template<typename F> static void runSessionDescriptorGenericGrammarTests(F callback) {
        SessionDescriptorGenericGrammar sdgg;
        /*
        unicast_address, multicast_address, IP4_multicast, m1, IP6_multicast,
        bm, ttl, FQDN, IP4_address, b1, IP6_address, hexpart, hexseq, hex4,
        extn_addr, text, byte_string, non_ws_string, token_char, token,
        email_safe, integer, alpha_numeric, POS_DIGIT, decimal_uchar;*/
        
        grammarTest("decimal-uchar", "0", sdgg.decimal_uchar, callback, true);
        grammarTest("decimal-uchar", "127", sdgg.decimal_uchar, callback, true);
        grammarTest("decimal-uchar", "255", sdgg.decimal_uchar, callback, true);
        grammarTest("!decimal-uchar", "-1", sdgg.decimal_uchar, callback, false);
        grammarTest("!decimal-uchar", "256", sdgg.decimal_uchar, callback, false);

        grammarTest("unicast-address", "127.0.0.1", sdgg.unicast_address, callback, true);
        grammarTest("unicast-address", "192.168.1.100", sdgg.unicast_address, callback, true);
        grammarTest("unicast-address", "218.186.8.238", sdgg.unicast_address, callback, true);
        grammarTest("unicast-address", "FE80::D69A:20FF:FE71:B84C", sdgg.unicast_address, callback, true);
        grammarTest("unicast-address", "www.g00-gl3.com", sdgg.unicast_address, callback, true);

        grammarTest("!multicast-address", "127.0.0.1", sdgg.multicast_address, callback, false);
        grammarTest("!multicast-address", "192.168.1.100", sdgg.multicast_address, callback, false);
        grammarTest("!multicast-address", "218.186.8.238", sdgg.multicast_address, callback, false);
        grammarTest("multicast-address", "FE80::D69A:20FF:FE71:B84C", sdgg.multicast_address, callback, true);
        grammarTest("multicast-address", "www.g00-gl3.com", sdgg.multicast_address, callback, true);
    }
    
    template<typename F> static void runSessionDescriptorGrammarTests(F callback) {
        /* proto_version, origin_field, session_name_field, information_field,
        uri_field, email_fields, phone_fields, connection_field,
        bandwidth_fields, time_fields, repeat_fields, zone_adjustments,
        key_field, attribute_fields, media_descriptions, media_field, username,
        sess_id, sess_version, nettype, addrtype, uri, email_address,
        address_and_comment, dispname_and_address, phone_number, phone,
        connection_address, bwtype, bandwidth, start_time, stop_time, time,
        repeat_interval, typed_time, fixed_len_time_unit, key_type, base64_char,
        base64_unit, base64_pad, base64, attribute, att_field, att_value, media,
        fmt, proto, port; */
        SessionDescriptorGrammar sdg;

        grammarTest("media-field", "m=audio 50016 RTP/AVP 0\r\n", sdg.media_field, callback, true);
        
        grammarTest("connection-field", "c=IN IP4 218.186.177.16\r\n", sdg.connection_field, callback, true);
        
        grammarTest("connection-address", "218.186.177.16", sdg.connection_address, callback, true);
        grammarTest("connection-address", "127.0.0.1", sdg.connection_address, callback, true);
        grammarTest("connection-address", "192.168.1.100", sdg.connection_address, callback, true);
        grammarTest("connection-address", "218.186.8.23", sdg.connection_address, callback, true);
        grammarTest("connection-address", "FE80::D69A:20FF:FE71:B84C", sdg.connection_address, callback, true);
        grammarTest("connection-address", "www.g00-gl3.com", sdg.connection_address, callback, true);
    }
};

class TestRunner {
    FB::JSObjectPtr jscb;
public:
    TestRunner(const FB::JSObjectPtr& jscb) : jscb(jscb) {}

    void operator()() {
        using namespace boost::phoenix;
        using namespace boost::phoenix::arg_names;

        GrammarTests::runTests(
            boost::phoenix::bind(&TestRunner::reportTestResult, *this, arg1, arg2));
    }

    void reportTestResult(
        const std::string& testName, 
        const std::string& testResult) {
        jscb->Invoke("", FB::variant_list_of(testName)(testResult));
    }
};

RegressionTests::RegressionTests() {
    registerMethod("runTests",
    	FB::make_method(this, &RegressionTests::runTests));
}

RegressionTests::~RegressionTests() {
}

void RegressionTests::runTests(const FB::JSObjectPtr& callback) {
    TestRunner tr(callback);
    boost::thread t(tr);
}