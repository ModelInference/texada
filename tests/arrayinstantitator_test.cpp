/*
 * arrayinstantitator_test.cpp
 *
 *  Created on: May 21, 2014
 *      Author: clemieux
 */

#include "../src/arrayinstantitator.h"
#include <gtest/gtest.h>
#include <ltlparse/public.hh>
#include <ltlvisit/apcollect.hh>


TEST(ArrayInstantiatorTest,TwoBindingsThreeEvents){
	//Set up to get the atomic proposition map from a formula
	 std::string input = "G(x -> Fy)";
	 spot::ltl::parse_error_list pel;
	 const spot::ltl::formula* f = spot::ltl::parse(input, pel);
	 spot::ltl::atomic_prop_set* formula_vars = spot::ltl::atomic_prop_collect(f);
	 // create a set of events
	 std::set<std::string> * events = new std::set<std::string>();
	 events->insert("a");
	 events->insert("b");
	 events->insert("c");
	 // now create the event instantiator
	texada::array_instantiator* instantiator = new texada::array_instantiator(events,formula_vars);
	texada::array_instantiator::inst_fxn* returned_array = instantiator->return_instantiations();
	ASSERT_EQ(returned_array[0].inst_map.at("x"),"a");
	ASSERT_EQ(returned_array[0].inst_map.at("y"),"a");
	ASSERT_EQ(returned_array[1].inst_map.at("x"),"b");
	ASSERT_EQ(returned_array[1].inst_map.at("y"),"a");
	ASSERT_EQ(returned_array[2].inst_map.at("x"),"c");
	ASSERT_EQ(returned_array[2].inst_map.at("y"),"a");
	ASSERT_EQ(returned_array[3].inst_map.at("x"),"a");
	ASSERT_EQ(returned_array[3].inst_map.at("y"),"b");
	ASSERT_EQ(returned_array[4].inst_map.at("x"),"b");
	ASSERT_EQ(returned_array[4].inst_map.at("y"),"b");
	ASSERT_EQ(returned_array[5].inst_map.at("x"),"c");
	ASSERT_EQ(returned_array[5].inst_map.at("y"),"b");
	ASSERT_EQ(returned_array[6].inst_map.at("x"),"a");
	ASSERT_EQ(returned_array[6].inst_map.at("y"),"c");
	ASSERT_EQ(returned_array[7].inst_map.at("x"),"b");
	ASSERT_EQ(returned_array[7].inst_map.at("y"),"c");
	ASSERT_EQ(returned_array[8].inst_map.at("x"),"c");
	ASSERT_EQ(returned_array[8].inst_map.at("y"),"c");


}

