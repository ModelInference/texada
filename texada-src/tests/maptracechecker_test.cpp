/*
 * maptracechecker_test.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: clemieux
 */

#include "../src/checkers/maptracechecker.h"
#include "../src/parsing/simpleparser.h"

#include <gtest/gtest.h>

#include <ltlparse/public.hh>
#include <ltlvisit/nenoform.hh>
#include <ltlvisit/tostring.hh>
#include <ltlenv/defaultenv.hh>
#include <ltlparse/public.hh>

#include <climits>
#include <fstream>
#include <stdlib.h>


// Testing map_trace_checker on a small trace, and trying to get decent code coverage
// for check and find first occ.
TEST(MapCheckerTest,SmallTrace){

    // create the trace map
	std::map<texada::string_event, std::vector<long>> trace_map;

	// create the position vector for a and insert into trace map
	texada::string_event aevent = texada::string_event("a");
	long aposns[] = {0,1};
	std::vector<long> apos_vec (aposns, aposns + sizeof(aposns) / sizeof(long) );
	trace_map.insert(std::pair<texada::string_event, std::vector<long>>(aevent,apos_vec));

	// create the position vector for b and insert into trace map
	texada::string_event bevent = texada::string_event("b");
	long bposns[] = {2,3};
	std::vector<long> bpos_vec (bposns, bposns + sizeof(bposns) / sizeof(long) );
	trace_map.insert(std::pair<texada::string_event, std::vector<long>>(bevent,bpos_vec));

	// create the position vector for terminal event and insert into trace map
	texada::string_event termvent = texada::string_event();
	std::vector<long> tpos_vec;
	tpos_vec.push_back(4);
	trace_map.insert(std::pair<texada::string_event, std::vector<long>>(termvent,tpos_vec));

	// create checker and parse error list to parse formula later on
	texada::map_trace_checker checker = texada::map_trace_checker(&trace_map);
	spot::ltl::parse_error_list pel;


	// Tests to try and cover the "check" functions

	std::string input = "G(a->Fb)";
	const spot::ltl::formula* f = spot::ltl::parse(input, pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	//until
	input = "a U b";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	//weak until
	input = "a W b";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	//release
	input = "b R a";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	//strong release
	input = "b M a";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	//xor
	input = "b xor a";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	//equiv, iff
	input = "b <-> a";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	//impliies
	input = "a -> b";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	input = "a -> Fb";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	//or
	input = "a | b";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	//and
	input = "a & b";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	//finally
	input = "Fa";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

    //globally
    input = "Ga";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE(checker.check_on_trace(f));

	// next
	input = "Xa";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	// not
	input = "!b";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	// tests to try and cover finding first occurrence

	// first occ next
	input = "G(X!a)";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	// first occ U
	input = "G(!a R !b)";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	//first occ W
	input = "G(!a M !b)";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	// first occ R
	input = "G(!b U !a)";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	// first occ M
	input = "G(!b W !a)";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	//first occ G
	input = "F(G b)";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	//first occ F
	input = "F(F a)";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	//first occ ->
	input = "F(a->b)";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE(checker.check_on_trace(f));

    //first occ xor
    input = "G(!(a xor b))";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE(checker.check_on_trace(f));

    //first occ <->
    input = "F((a<->b))";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE(checker.check_on_trace(f));
}

// testing whether map trace mines a more complex formula correctly.
TEST(MapCheckerTest,ResourceAllocation){

    std::string texada_home = std::string(getenv("TEXADA_HOME"));

    std::string file_source = texada_home + "/traces/resource-allocation/smallabc.txt";

	// parse the ltl formula
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* formula = spot::ltl::parse("(!(b | c) W a) & G((b -> XFc) & (a -> X((!a U c) & (!c U b))) & (c -> X(!(b | c) W a)))", pel);

	// parse log file
	std::ifstream infile(file_source);
	texada::simple_parser * parser =  new texada::simple_parser();
	parser->parse_to_map(infile);
	std::set<std::map<texada::string_event,std::vector<long>> >  trace_set = parser->return_map_trace();
	std::set<std::string>  event_set = parser->return_events();

	// check
	texada::map_trace_checker checker = texada::map_trace_checker(&(*trace_set.begin()));
	ASSERT_TRUE(checker.check_on_trace(formula));
}




