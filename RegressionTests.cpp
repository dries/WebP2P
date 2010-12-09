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
};

class TestRunner {
    FB::JSObjectPtr jscb;
public:
    TestRunner(const FB::JSObjectPtr& jscb) : jscb(jscb) {}

    void operator()() {
        using namespace boost::phoenix;
        using namespace boost::phoenix::arg_names;

        GrammarTests::runTests(
            bind(&TestRunner::reportTestResult, *this, arg1, arg2));
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