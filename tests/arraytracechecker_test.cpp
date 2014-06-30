/*
 * arraytracechecker2_test.cpp
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 *
 *      Right now this class is just a junk class as I try to figure things out.
 */

#include "../src/arraytracechecker.h"
#include <ltlparse/public.hh>
#include <ltlvisit/tostring.hh>
#include <gtest/gtest.h>
#include "../src/stringevent.h"

TEST(ArrayTraceCheckerTest, AFby){
	texada::string_event* trace = new texada::string_event[5];
	trace[0] = *(new texada::string_event("a",false));
	trace[1] = *(new texada::string_event("a",false));
	trace[2] = *(new texada::string_event("b",false));
	trace[3] = *(new texada::string_event("b",false));
	trace[4] = *(new texada::string_event("EndOfTraceVar",true));

	std::string input = "G(a->Fb)";
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* f = spot::ltl::parse(input, pel);

	texada::array_trace_checker* checker = new texada::array_trace_checker();

	ASSERT_TRUE(checker->check(f,trace)) << "Test G(a->Fb) on [a,a,b,b] returned false.";

	input = "G(a->Fa)";
	f = spot::ltl::parse(input, pel);
	ASSERT_TRUE(checker->check(f,trace)) << "Test G(a->Fa) on [a,a,b,b] returned false.";

	input = "G(a->XFa)";
	f = spot::ltl::parse(input, pel);
	ASSERT_FALSE(checker->check(f,trace)) << "Test G(a->XFa) on [a,a,b,b] returned true.";

	input = "G(b->Fa)";
	f = spot::ltl::parse(input, pel);
	ASSERT_FALSE(checker->check(f,trace)) << "Test G(b->Fa) on [a,a,b,b] returned true.";


}

TEST(ArrayTraceCheckerTest, NextNext){
	texada::string_event* trace = new texada::string_event[5];
	trace[0] = *(new texada::string_event("a",false));
	trace[1] = trace[0];
	trace[2] = *(new texada::string_event("b",false));
	trace[3] = *(new texada::string_event("b",false));
	trace[4] = *(new texada::string_event("EndOfTraceVar",true));

	//std::cout << "Does the next next make you unhappy... \n";
	std::string input = "G(b->XXa)";
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* f = spot::ltl::parse(input, pel);

	texada::array_trace_checker* checker = new texada::array_trace_checker();

	ASSERT_FALSE(checker->check(f,trace));

	//std::cout << "Or the next next not? \n";
	input = "G(b->XX!a)";
	f = spot::ltl::parse(input, pel);
	ASSERT_TRUE(checker->check(f,trace));



}

TEST(ArrayTraceCheckerTest,Alternating){
	texada::string_event* psSucceed = new texada::string_event[5];
	psSucceed[0] = *(new texada::string_event("p",false));
	psSucceed[1] = *(new texada::string_event("s",false));
	psSucceed[2] = *(new texada::string_event("p",false));
	psSucceed[3] = *(new texada::string_event("s",false));
	psSucceed[4] = *(new texada::string_event("EndOfTraceVar",true));

	texada::string_event* psFail = new texada::string_event[6];
	psFail[0] = *(new texada::string_event("p",false));
	psFail[1] = *(new texada::string_event("s",false));
	psFail[2] = *(new texada::string_event("p",false));
	psFail[3] = *(new texada::string_event("s",false));
	psFail[4] = *(new texada::string_event("p",false));
	psFail[5] = *(new texada::string_event("EndOfTraceVar",true));;

	std::string alti = "(!s W p) & G((p->X(!p U s)) & (s ->X(!s W p)))";
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* altf = spot::ltl::parse(alti, pel);

	texada::array_trace_checker* checker = new texada::array_trace_checker();

	ASSERT_TRUE(checker->check(altf,psSucceed));

	ASSERT_FALSE(checker->check(altf,psFail));


}
TEST(ArrayTraceCheckerTest,Until){
	texada::string_event* trace = new texada::string_event[2];
	trace[0] = *(new texada::string_event("q",false));
	trace[1] = *(new texada::string_event("EndOfTraceVar",true));

	std::string input = "p U q";
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* f = spot::ltl::parse(input, pel);

	texada::array_trace_checker* checker = new texada::array_trace_checker();

	ASSERT_TRUE(checker->check(f,trace));

	input = "!p U q";
	f= spot::ltl::parse(input,pel);

	ASSERT_TRUE(checker->check(f,trace));

}
