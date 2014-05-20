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

/*
int main(){
	std::cout << "Simple AFby test: \n";
	std::string* trace = new std::string[5];
	trace[0] = "a";
	trace[1] = "a";
	trace[2] = "b";
	trace[3] = "b";
	trace[4] = "EndOfTraceVar";

	std::string input = "G(a->Fb)";
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* f = spot::ltl::parse(input, pel);
	std::cout << spot::ltl::to_string(f) << ": ";

	texada::array_trace_checker* checker = new texada::array_trace_checker();

	std::cout << checker->check(f,trace) << "\n";

	input = "G(a->Fa)";
	f = spot::ltl::parse(input, pel);
	std::cout << spot::ltl::to_string(f) << ": ";
	std::cout << checker->check(f,trace) << "\n";

	input = "G(a->XFa)";
	f = spot::ltl::parse(input, pel);
	std::cout << spot::ltl::to_string(f) << ": ";
	std::cout << checker->check(f,trace) << "\n";

	input = "G(b->Fa)";
	f = spot::ltl::parse(input, pel);
	std::cout << spot::ltl::to_string(f) << ": ";
	std::cout << checker->check(f,trace) << "\n";

	/*
	input = "F(b->(Xb&XXb& XXXb))";
	f = spot::ltl::parse(input, pel);
	std::cout << spot::ltl::to_string(f) << ": ";
	std::cout << checker->check(f,trace) << "\n";
	*//*

	std::string* psSucceed = new std::string[5];
	psSucceed[0] = "p";
	psSucceed[1] = "s";
	psSucceed[2] = "p";
	psSucceed[3] = "s";
	psSucceed[4] = "EndOfTraceVar";

	std::string* psFail = new std::string[6];
	psFail[0] = "p";
	psFail[1] = "s";
	psFail[2] = "p";
	psFail[3] = "s";
	psFail[4] = "p";
	psFail[5] = "EndOfTraceVar";

	std::string alti = "(!s W p) & G((p->X(!p U s)) & (s ->X(!s W p)))";
	const spot::ltl::formula* altf = spot::ltl::parse(alti, pel);
	std::cout << spot::ltl::to_string(altf) << ": \n";

	std::cout << "On succeeding trace: " << checker->check(altf,psSucceed) << "\n";

	std::cout << "On failing trace: " << checker->check(altf,psFail) << "\n";

}*/
