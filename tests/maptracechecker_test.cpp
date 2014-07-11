/*
 * maptracechecker_test.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: clemieux
 */

#include "../src/maptracechecker.h"
#include "../src/simpleparser.h"
#include <gtest/gtest.h>
#include <ltlparse/public.hh>
#include <ltlvisit/nenoform.hh>
#include <ltlvisit/tostring.hh>
#include <ltlenv/defaultenv.hh>
#include <ltlparse/public.hh>
#include <climits>
#include <fstream>


/**
 * need both interval and find_first_occurence(const spot::ltl::atomic_prop*,interval) to be
 * public for this test to run
 *//*
TEST(SearchCheckerTest,AtomicProp){

	std::map<texada::string_event, std::vector<long>> trace_map;
	texada::string_event aevent = texada::string_event("a",false);
	long posns[] = {3,16,19,56,61,63,76,97};
	std::vector<long> pos_vec (posns, posns + sizeof(posns) / sizeof(long) );
	trace_map.insert(std::pair<texada::string_event, std::vector<long>>(aevent,pos_vec));

	const spot::ltl::atomic_prop * aprop =
	static_cast<const spot::ltl::atomic_prop*>(spot::ltl::default_environment::instance().require("a"));

	texada::map_trace_checker checker = texada::map_trace_checker(trace_map);

	texada::map_trace_checker::interval intvl;

	//Finding somewhere in the middle
	intvl.start= 14;
	intvl.end= 68;
	ASSERT_EQ(16,checker.find_first_occurrence(aprop,intvl));

	//Finding somewhere else in the middle
	intvl.start = 58;
	ASSERT_EQ(61,checker.find_first_occurrence(aprop,intvl));

	//Finding in the middle, target is start
	intvl.start = 61;
	ASSERT_EQ(61,checker.find_first_occurrence(aprop,intvl));

	//Failing to find in the middle, interval is between two occurrences;
	intvl.start = 20;
	intvl.end = 40;
	ASSERT_EQ(-1,checker.find_first_occurrence(aprop,intvl));

	//Finding in the middle, full interval
	intvl.start = 0;
	intvl.end = LONG_MAX;
	ASSERT_EQ(3,checker.find_first_occurrence(aprop,intvl));

	//Failing to find: entire interval is before first occurrence
	intvl.end = 2;
	ASSERT_EQ(-1,checker.find_first_occurrence(aprop,intvl));

	//First occurrence is last
	intvl.start = 90;
	intvl.end = LONG_MAX;
	ASSERT_EQ(97,checker.find_first_occurrence(aprop,intvl));

	//Failing because entire interval is after last occurrence
	intvl.start = 98;
	ASSERT_EQ(-1,checker.find_first_occurrence(aprop,intvl));

	//Failing between last two elements
	intvl.start = 80;
	intvl.end = 85;
	ASSERT_EQ(-1,checker.find_first_occurrence(aprop,intvl));

}*/

TEST(MapCheckerTest,SmallTrace){
	std::map<texada::string_event, std::vector<long>> trace_map;
	texada::string_event aevent = texada::string_event("a");
	long aposns[] = {0,1};
	std::vector<long> apos_vec (aposns, aposns + sizeof(aposns) / sizeof(long) );
	trace_map.insert(std::pair<texada::string_event, std::vector<long>>(aevent,apos_vec));
	texada::string_event bevent = texada::string_event("b");
	long bposns[] = {2,3};
	std::vector<long> bpos_vec (bposns, bposns + sizeof(bposns) / sizeof(long) );
	trace_map.insert(std::pair<texada::string_event, std::vector<long>>(bevent,bpos_vec));
	texada::string_event termvent = texada::string_event();
	std::vector<long> tpos_vec;
	tpos_vec.push_back(4);
	trace_map.insert(std::pair<texada::string_event, std::vector<long>>(termvent,tpos_vec));




	texada::map_trace_checker checker = texada::map_trace_checker(&trace_map);
	spot::ltl::parse_error_list pel;

	/*
	const spot::ltl::formula* test = spot::ltl::parse("a R b", pel);
	const spot::ltl::formula* nf = spot::ltl::negative_normal_form(test,true);
	std::cout << spot::ltl::to_string(nf) << "\n";*/

	//tests for top level check.

	std::string input = "G(a->Fb)";
	const spot::ltl::formula* f = spot::ltl::parse(input, pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	input = "a U b";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	input = "a W b";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	input = "b R a";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	input = "b M a";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	input = "b xor a";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	input = "b <-> a";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	input = "a -> b";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	input = "a -> Fb";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	input = "a | b";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	input = "a & b";
	f = spot::ltl::parse(input,pel);
	ASSERT_FALSE(checker.check_on_trace(f));

	input = "Fa";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	input = "Xa";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	input = "!b";
	f = spot::ltl::parse(input,pel);
	ASSERT_TRUE(checker.check_on_trace(f));

	//tests for find_first_occ

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
}

TEST(MapCheckerTest,ResourceAllocation){

	//parse the ltl formula
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* formula = spot::ltl::parse("(!(b | c) W a) & G((b -> XFc) & (a -> X((!a U c) & (!c U b))) & (c -> X(!(b | c) W a)))", pel);

	std::ifstream infile("/home/clemieux/workspace/texada/Texada/traces/resource-allocation/smallabc.txt");
	texada::simple_parser * parser =  new texada::simple_parser();
	parser->parse_to_map(infile);
	std::set<std::map<texada::string_event,std::vector<long>> >  trace_set = parser->return_map_trace();
	std::set<std::string>  event_set = parser->return_events();

	texada::map_trace_checker checker = texada::map_trace_checker(&(*trace_set.begin()));
	ASSERT_TRUE(checker.check_on_trace(formula));
}




