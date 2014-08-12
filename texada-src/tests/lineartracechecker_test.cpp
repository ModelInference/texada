/*
 * lineartracechecker_test.cpp
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 *
 */

#include "../src/checkers/lineartracechecker.h"
#include "../src/parsing/stringevent.h"

#include <ltlparse/public.hh>
#include <ltlvisit/tostring.hh>

#include <gtest/gtest.h>

/**
 * Checks some border cases for the linear trace checker.
 */


// Checks AFby and variations of it on a small trace.
TEST(LinearTraceCheckerTest, AFby) {

    // create trace a a b b
    std::vector<texada::string_event> trace_vec;
    trace_vec.push_back(texada::string_event("a"));
    trace_vec.push_back(texada::string_event("a"));
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event());
    texada::string_event* trace = &trace_vec[0];

    texada::linear_trace_checker::trace_node trace_node;
    boost::get<const texada::string_event*>(trace_node) = trace;

    // parse the formula
    std::string input = "G(a->Fb)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);

    // create a new linear checker

    texada::linear_trace_checker* checker = new texada::linear_trace_checker();

    // G(a->Fb) should hold on the trace
    ASSERT_TRUE(checker->check(f,trace_node));
    f->destroy();

    // G(a->Fa) also holds because F includes the present as well as the future
    input = "G(a->Fa)";
    f = spot::ltl::parse(input, pel);
    ASSERT_TRUE(checker->check(f,trace_node));
    f->destroy();

    // Adding the next operator makes F talk exclusively about future events;
    // G(a->XFa) does not hold
    input = "G(a->XFa)";
    f = spot::ltl::parse(input, pel);
    ASSERT_FALSE(checker->check(f,trace_node));
    f->destroy();

    // b is not always followed by a.
    input = "G(b->Fa)";
    f = spot::ltl::parse(input, pel);
    ASSERT_FALSE(checker->check(f,trace_node));
    f->destroy();

    //clean up
    delete checker;
}

// checks if the next operator works properly on terminal events.
TEST(LinearTraceCheckerTest, NextNext) {
    // create trace a a b b
    std::vector<texada::string_event> trace_vec;
    trace_vec.push_back(texada::string_event("a"));
    trace_vec.push_back(texada::string_event("a"));
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event());
    texada::string_event* trace = &trace_vec[0];

    texada::linear_trace_checker::trace_node trace_node;
    boost::get<const texada::string_event*>(trace_node) = trace;

    // set up checker and parse error list necessary for parsing and checking
    texada::linear_trace_checker* checker = new texada::linear_trace_checker();
    spot::ltl::parse_error_list pel;

    // G(b->XXa) should force the checker to go "beyond" the end of the trace
    // and should return false since the terminal event, which is infinitely
    // appended to the end of the trace, is not a.
    std::string input = "G(b->XXa)";
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);
    ASSERT_FALSE(checker->check(f, trace_node));
    f->destroy();

    // G(b->XX!a) should return true because the terminal event is not a
    input = "G(b->XX!a)";
    f = spot::ltl::parse(input, pel);
    ASSERT_TRUE(checker->check(f, trace_node));
    f->destroy();

    // clean up
    delete checker;

}

// another test to check next behaviour on the terminal event
TEST(LinearTraceCheckerTest,Alternating) {

    // trace on which alternating holds : p s p s
    std::vector<texada::string_event> trace_vec;
    trace_vec.push_back(texada::string_event("p"));
    trace_vec.push_back(texada::string_event("s"));
    trace_vec.push_back(texada::string_event("p"));
    trace_vec.push_back(texada::string_event("s"));
    trace_vec.push_back(texada::string_event());
    texada::string_event* psSucceed = &trace_vec[0];

    texada::linear_trace_checker::trace_node trace_node1;
    boost::get<const texada::string_event*>(trace_node1) = psSucceed;

    // trace on which alternating does not hold: p s p s p
    std::vector<texada::string_event> trace_vec2;
    trace_vec2.push_back(texada::string_event("p"));
    trace_vec2.push_back(texada::string_event("s"));
    trace_vec2.push_back(texada::string_event("p"));
    trace_vec2.push_back(texada::string_event("s"));
    trace_vec2.push_back(texada::string_event("p"));
    trace_vec2.push_back(texada::string_event());
    texada::string_event* psFail = &trace_vec2[0];

    texada::linear_trace_checker::trace_node trace_node2;
    boost::get<const texada::string_event*>(trace_node2) = psFail;

    // alternating input in string form
    std::string alti = "((!s)W p)&G((p->X((!p)U s))&(s->X((!s)W p)))";
    spot::ltl::parse_error_list pel;
    // alternating formula is parsed from alternating input
    const spot::ltl::formula* altf = spot::ltl::parse(alti, pel);

    // create checker
    texada::linear_trace_checker* checker = new texada::linear_trace_checker();

    // alternating formula should hold on p s p s: we check !s W p on
    // the terminal event
    ASSERT_TRUE(checker->check(altf, trace_node1));

    // alternating formula will be false on p s p s p: we check !p U s
    // on the terminal event (it's false b/c s never occurs)
    ASSERT_FALSE(checker->check(altf, trace_node2));


    //clean up
    delete checker;
    altf->destroy();

}

// checking that p U q holds on the one-event trace q
TEST(LinearTraceCheckerTest,Until) {
    // create trace
    std::vector<texada::string_event> trace_vec;
    trace_vec.push_back(texada::string_event("q"));
    trace_vec.push_back(texada::string_event());
    texada::string_event* trace = &trace_vec[0];

    texada::linear_trace_checker::trace_node trace_node;
    boost::get<const texada::string_event*>(trace_node) = trace;

    //parse formula
    std::string input = "p U q";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);

    texada::linear_trace_checker* checker = new texada::linear_trace_checker();

    // check p U q is true
    ASSERT_TRUE(checker->check(f, trace_node));
    f->destroy();

    input = "!p U q";
    f = spot::ltl::parse(input, pel);

    // !p U q should also be true
    ASSERT_TRUE(checker->check(f, trace_node));
    f->destroy();

    //clean up
    delete checker;

}
