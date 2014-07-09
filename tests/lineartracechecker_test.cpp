/*
 * lineartracechecker_test.cpp
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 *
 *      Right now this class is just a junk class as I try to figure things out.
 */

#include "../src/lineartracechecker.h"
#include <ltlparse/public.hh>
#include <ltlvisit/tostring.hh>
#include <gtest/gtest.h>
#include "../src/stringevent.h"

TEST(LinearTraceCheckerTest, AFby) {

    std::vector<texada::string_event> trace_vec;
    trace_vec.push_back(texada::string_event("a", false));
    trace_vec.push_back(texada::string_event("a", false));
    trace_vec.push_back(texada::string_event("b", false));
    trace_vec.push_back(texada::string_event("b", false));
    trace_vec.push_back(texada::string_event("EndOfTraceVar", true));
    texada::string_event* trace = &trace_vec[0];

    std::string input = "G(a->Fb)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);

    texada::linear_trace_checker* checker = new texada::linear_trace_checker();

    ASSERT_TRUE(checker->check(f,trace))<< "Test G(a->Fb) on [a,a,b,b] returned false.";

    input = "G(a->Fa)";
    f = spot::ltl::parse(input, pel);
    ASSERT_TRUE(checker->check(f,trace))<< "Test G(a->Fa) on [a,a,b,b] returned false.";

    input = "G(a->XFa)";
    f = spot::ltl::parse(input, pel);
    ASSERT_FALSE(checker->check(f,trace))<< "Test G(a->XFa) on [a,a,b,b] returned true.";

    input = "G(b->Fa)";
    f = spot::ltl::parse(input, pel);
    ASSERT_FALSE(checker->check(f,trace))<< "Test G(b->Fa) on [a,a,b,b] returned true.";

    delete checker;
}

TEST(LinearTraceCheckerTest, NextNext) {
    std::vector<texada::string_event> trace_vec;
    trace_vec.push_back(texada::string_event("a", false));
    trace_vec.push_back(texada::string_event("a", false));
    trace_vec.push_back(texada::string_event("b", false));
    trace_vec.push_back(texada::string_event("b", false));
    trace_vec.push_back(texada::string_event("EndOfTraceVar", true));
    texada::string_event* trace = &trace_vec[0];

    //std::cout << "Does the next next make you unhappy... \n";
    std::string input = "G(b->XXa)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);

    texada::linear_trace_checker* checker = new texada::linear_trace_checker();

    ASSERT_FALSE(checker->check(f, trace));

    //std::cout << "Or the next next not? \n";
    input = "G(b->XX!a)";
    f = spot::ltl::parse(input, pel);
    ASSERT_TRUE(checker->check(f, trace));

    delete checker;

}

TEST(LinearTraceCheckerTest,Alternating) {
    std::vector<texada::string_event> trace_vec;
    trace_vec.push_back(texada::string_event("p", false));
    trace_vec.push_back(texada::string_event("s", false));
    trace_vec.push_back(texada::string_event("p", false));
    trace_vec.push_back(texada::string_event("s", false));
    trace_vec.push_back(texada::string_event("EndOfTraceVar", true));
    texada::string_event* psSucceed = &trace_vec[0];

    std::vector<texada::string_event> trace_vec2;
    trace_vec2.push_back(texada::string_event("p", false));
    trace_vec2.push_back(texada::string_event("s", false));
    trace_vec2.push_back(texada::string_event("p", false));
    trace_vec2.push_back(texada::string_event("s", false));
    trace_vec2.push_back(texada::string_event("p", false));
    trace_vec2.push_back(texada::string_event("EndOfTraceVar", true));
    texada::string_event* psFail = &trace_vec2[0];

    std::string alti = "((!s)W p)&G((p->X((!p)U s))&(s->X((!s)W p)))";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* altf = spot::ltl::parse(alti, pel);


    texada::linear_trace_checker* checker = new texada::linear_trace_checker();

    ASSERT_TRUE(checker->check(altf, psSucceed));

    ASSERT_FALSE(checker->check(altf, psFail));

    delete checker;

}
TEST(LinearTraceCheckerTest,Until) {
    std::vector<texada::string_event> trace_vec;
    trace_vec.push_back(texada::string_event("q", false));
    trace_vec.push_back(texada::string_event("EndOfTraceVar", true));
    texada::string_event* trace = &trace_vec[0];

    std::string input = "p U q";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);

    texada::linear_trace_checker* checker = new texada::linear_trace_checker();

    ASSERT_TRUE(checker->check(f, trace));

    input = "!p U q";
    f = spot::ltl::parse(input, pel);

    ASSERT_TRUE(checker->check(f, trace));

    delete checker;

}
