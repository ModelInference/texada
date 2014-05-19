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
	std::cout << spot::ltl::to_string(f) << "\n";

	texada::array_trace_checker* checker = new texada::array_trace_checker();

	std::cout << checker->check(f,trace);

	input = "G(b->Fa)";
	f = spot::ltl::parse(input, pel);
	std::cout << checker->check(f,trace);

}
