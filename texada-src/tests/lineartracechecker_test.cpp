/*
 * lineartracechecker_test.cpp
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 *
 */

#include "../src/checkers/lineartracechecker.h"
#include "../src/trace/event.h"
#include "../src/checkers/statistic.h"
#include "../src/checkers/settings.h"

#include "../src/instantiation-tools/texadatospotmapping.h"
#include "../src/instantiation-tools/texadasimplify.h"

#include <gtest/gtest.h>

/**
 * Checks some border cases for the linear trace checker.
 */

// Checks AFby and variations of it on a small trace.
TEST(LinearTraceCheckerTest, AFby) {

    // create trace a a b b
    std::vector<texada::event> trace_vec;
    trace_vec.push_back(texada::event("a"));
    trace_vec.push_back(texada::event("a"));
    trace_vec.push_back(texada::event("b"));
    trace_vec.push_back(texada::event("b"));
    trace_vec.push_back(texada::event());
    texada::event* trace = &trace_vec[0];

    // parse the formula
    std::string input = "G(a->Fb)";
    texada::ltl::parse_error_list pel;
    const texada::ltl::formula* f = texada::ltl::parse(input, pel);

    // create a new linear checker

    texada::linear_trace_checker* checker = new texada::linear_trace_checker();

    // G(a->Fb) should hold on the trace
    ASSERT_TRUE((checker->check_on_trace(f,trace)).is_satisfied);
    f->destroy();

    // G(a->Fa) also holds because F includes the present as well as the future
    input = "G(a->Fa)";
    f = texada::ltl::parse(input, pel);
    ASSERT_TRUE((checker->check_on_trace(f,trace)).is_satisfied);
    f->destroy();

    // Adding the next operator makes F talk exclusively about future events;
    // G(a->XFa) does not hold
    input = "G(a->XFa)";
    f = texada::ltl::parse(input, pel);
    ASSERT_FALSE((checker->check_on_trace(f,trace)).is_satisfied);
    f->destroy();

    // b is not always followed by a.
    input = "G(b->Fa)";
    f = texada::ltl::parse(input, pel);
    ASSERT_FALSE((checker->check_on_trace(f,trace)).is_satisfied);
    f->destroy();

    //clean up
    delete checker;
}

// checks if the next operator works properly on terminal events.
TEST(LinearTraceCheckerTest, NextNext) {

    // create trace a a b b
    std::vector<texada::event> trace_vec;
    trace_vec.push_back(texada::event("a"));
    trace_vec.push_back(texada::event("a"));
    trace_vec.push_back(texada::event("b"));
    trace_vec.push_back(texada::event("b"));
    trace_vec.push_back(texada::event());
    texada::event* trace = &trace_vec[0];

    // set up checker and parse error list necessary for parsing and checking
    texada::linear_trace_checker* checker = new texada::linear_trace_checker();
    texada::ltl::parse_error_list pel;

    // G(b->XXa) should force the checker to go "beyond" the end of the trace
    // and should return false since the terminal event, which is infinitely
    // appended to the end of the trace, is not a.
    std::string input = "G(b->XXa)";
    const texada::ltl::formula* f = texada::ltl::parse(input, pel);
    ASSERT_FALSE((checker->check_on_trace(f,trace)).is_satisfied);
    f->destroy();

    // G(b->XX!a) should return true because the terminal event is not a
    input = "G(b->XX!a)";
    f = texada::ltl::parse(input, pel);
    ASSERT_TRUE((checker->check_on_trace(f,trace)).is_satisfied);
    f->destroy();

    // clean up
    delete checker;

}

// another test to check next behaviour on the terminal event
TEST(LinearTraceCheckerTest,Alternating) {

    // trace on which alternating holds : p s p s
    std::vector<texada::event> trace_vec;
    trace_vec.push_back(texada::event("p"));
    trace_vec.push_back(texada::event("s"));
    trace_vec.push_back(texada::event("p"));
    trace_vec.push_back(texada::event("s"));
    trace_vec.push_back(texada::event());
    texada::event* psSucceed = &trace_vec[0];

    // trace on which alternating does not hold: p s p s p
    std::vector<texada::event> trace_vec2;
    trace_vec2.push_back(texada::event("p"));
    trace_vec2.push_back(texada::event("s"));
    trace_vec2.push_back(texada::event("p"));
    trace_vec2.push_back(texada::event("s"));
    trace_vec2.push_back(texada::event("p"));
    trace_vec2.push_back(texada::event());
    texada::event* psFail = &trace_vec2[0];

    // alternating input in string form
    std::string alti = "((!s)W p)&G((p->X((!p)U s))&(s->X((!s)W p)))";
    texada::ltl::parse_error_list pel;
    // alternating formula is parsed from alternating input
    const texada::ltl::formula* altf = texada::ltl::parse(alti, pel);

    // create checker
    texada::linear_trace_checker* checker = new texada::linear_trace_checker();

    // alternating formula should hold on p s p s: we check !s W p on
    // the terminal event
    ASSERT_TRUE((checker->check_on_trace(altf, psSucceed)).is_satisfied);

    // alternating formula will be false on p s p s p: we check !p U s
    // on the terminal event (it's false b/c s never occurs)
    ASSERT_FALSE((checker->check_on_trace(altf, psFail)).is_satisfied);

    //clean up
    delete checker;
    altf->destroy();

}

// checking that p U q holds on the one-event trace q
TEST(LinearTraceCheckerTest,Until) {
    // create trace
    std::vector<texada::event> trace_vec;
    trace_vec.push_back(texada::event("q"));
    trace_vec.push_back(texada::event());
    texada::event* trace = &trace_vec[0];

    //parse formula
    std::string input = "p U q";
    texada::ltl::parse_error_list pel;
    const texada::ltl::formula* f = texada::ltl::parse(input, pel);

    texada::linear_trace_checker* checker = new texada::linear_trace_checker();

    // check p U q is true
    ASSERT_TRUE((checker->check_on_trace(f, trace)).is_satisfied);
    f->destroy();

    input = "!p U q";
    f = texada::ltl::parse(input, pel);

    // !p U q should also be true
    ASSERT_TRUE((checker->check_on_trace(f, trace)).is_satisfied);
    f->destroy();

    //clean up
    delete checker;

}

// checking support and support potential of property instances are computed properly
TEST(LinearTraceCheckerTest, SupAndSupPot) {

    // create trace a b a b a c
    std::vector<texada::event> trace_vec;
    trace_vec.push_back(texada::event("a"));
    trace_vec.push_back(texada::event("a"));
    trace_vec.push_back(texada::event("c"));
    trace_vec.push_back(texada::event("a"));
    trace_vec.push_back(texada::event("b"));
    trace_vec.push_back(texada::event("a"));
    trace_vec.push_back(texada::event());
    texada::event* trace = &trace_vec[0];

    texada::linear_trace_checker* checker = new texada::linear_trace_checker();
    checker->configure(texada::settings(0,0,1.0,false,true));
    std::unique_ptr<texada::ltl::ltl_simplifier> simplifier(new texada::ltl::ltl_simplifier());

    // Tests to try and cover the support and support-potential measurements of findings

    std::string input = "G(a->Fb)";
    texada::ltl::parse_error_list pel;
    const texada::ltl::formula* f = texada::ltl::parse(input, pel);
    ASSERT_EQ(3, checker->check_on_trace(f, trace).support);
    ASSERT_EQ(4, checker->check_on_trace(f, trace).support_potential);
    f->destroy();

    //until
    input = "a U b";
    f = texada::ltl::parse(input,pel);
    ASSERT_EQ(4, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(5, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    //weak until
    input = "a W b";
    f = texada::ltl::parse(input,pel);
    ASSERT_EQ(3, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(4, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    //release
    input = "b R a";
    f = texada::ltl::parse(input,pel);
    ASSERT_EQ(3, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(5, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    //strong release
    input = "b M a";
    f = texada::ltl::parse(input,pel);
    ASSERT_EQ(4, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(6, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    //xor
    input = "b xor a";
    f = texada::ltl::parse(input,pel);
    const texada::ltl::formula * to_delete = f;
    f = simplifier->negative_normal_form(f);
    to_delete->destroy();
    ASSERT_EQ(2, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(2, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    //equiv, iff
    input = "b <-> a";
    f = texada::ltl::parse(input,pel);
    to_delete = f;
    f = simplifier->negative_normal_form(f);
    to_delete->destroy();
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(2, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    //implies
    input = "a -> b";
    f = texada::ltl::parse(input,pel);
    to_delete = f;
    f = simplifier->negative_normal_form(f);
    to_delete->destroy();
    ASSERT_EQ(0, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    input = "a -> Fb";
    f = texada::ltl::parse(input,pel);
    to_delete = f;
    f = simplifier->negative_normal_form(f);
    to_delete->destroy();
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    //or
    input = "a | b";
    f = texada::ltl::parse(input,pel);
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    //and
    input = "a & b";
    f = texada::ltl::parse(input,pel);
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(2, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    //finally
    input = "Fa";
    f = texada::ltl::parse(input,pel);
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    //globally
    input = "Ga";
    f = texada::ltl::parse(input,pel);
    ASSERT_EQ(4, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(6, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    //never
    input = "!Ga";
    f = texada::ltl::parse(input,pel);
    to_delete = f;
    f = simplifier->negative_normal_form(f);
    to_delete->destroy();
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    // next
    input = "Xa";
    f = texada::ltl::parse(input,pel);
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    // not
    input = "!b";
    f = texada::ltl::parse(input,pel);
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support);
    ASSERT_EQ(1, (checker->check_on_trace(f, trace)).support_potential);
    f->destroy();

    //clean up
    delete checker;
}
