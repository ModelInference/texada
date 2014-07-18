/*
 * instantspoolcreator_test.cpp
 *
 *  Created on: May 21, 2014
 *      Author: clemieux
 */

#include "../src/instantiation-tools/instantspoolcreator.h"
#include <gtest/gtest.h>
#include <ltlparse/public.hh>
#include <ltlvisit/apcollect.hh>

/**
 * Tests the creation of the pool of instantiation functions
 * by checking if the returned array contains all 2-event
 * permutations with repetition of a,b,c.
 */

TEST(InstantiatorPoolCreatorTest,TwoBindingsThreeEvents){
	//Set up to get the atomic propositions from a property type
	 std::string input = "G(x -> Fy)";
	 spot::ltl::parse_error_list pel;
	 const spot::ltl::formula* f = spot::ltl::parse(input, pel);
	 spot::ltl::atomic_prop_set* formula_vars = spot::ltl::atomic_prop_collect(f);
	 // create a set of events
	 std::set<std::string> events =  std::set<std::string>();
	 events.insert("a");
	 events.insert("b");
	 events.insert("c");
	 // now create the event instantiator
	texada::instants_pool_creator instantiator = texada::instants_pool_creator(events,*formula_vars);
	instantiator.instantiate_array();
	std::shared_ptr<std::vector<texada::instants_pool_creator::inst_fxn>> returned_array = instantiator.return_instantiations();
	//check that all expected mappings occur
	ASSERT_EQ(returned_array->at(0).inst_map.at("x"),"a");
	ASSERT_EQ(returned_array->at(0).inst_map.at("y"),"a");
	ASSERT_EQ(returned_array->at(1).inst_map.at("x"),"b");
	ASSERT_EQ(returned_array->at(1).inst_map.at("y"),"a");
	ASSERT_EQ(returned_array->at(2).inst_map.at("x"),"c");
	ASSERT_EQ(returned_array->at(2).inst_map.at("y"),"a");
	ASSERT_EQ(returned_array->at(3).inst_map.at("x"),"a");
	ASSERT_EQ(returned_array->at(3).inst_map.at("y"),"b");
	ASSERT_EQ(returned_array->at(4).inst_map.at("x"),"b");
	ASSERT_EQ(returned_array->at(4).inst_map.at("y"),"b");
	ASSERT_EQ(returned_array->at(5).inst_map.at("x"),"c");
	ASSERT_EQ(returned_array->at(5).inst_map.at("y"),"b");
	ASSERT_EQ(returned_array->at(6).inst_map.at("x"),"a");
	ASSERT_EQ(returned_array->at(6).inst_map.at("y"),"c");
	ASSERT_EQ(returned_array->at(7).inst_map.at("x"),"b");
	ASSERT_EQ(returned_array->at(7).inst_map.at("y"),"c");
	ASSERT_EQ(returned_array->at(8).inst_map.at("x"),"c");
	ASSERT_EQ(returned_array->at(8).inst_map.at("y"),"c");
	delete formula_vars;
	f->destroy();


}

