/*
 * apsubbingcloner_test.cpp
 *
 *  Created on: May 20, 2014
 *      Author: clemieux
 *
 *  Test the atomic proposition subbing cloner: does it correctly
 *  substitute the original variables of the spot formula with those provided
 */

#include "../src/instantiation-tools/apsubbingcloner.h"

#include <ltlparse/public.hh>
#include <ltlvisit/tostring.hh>
#include <map>
#include <string>
#include <gtest/gtest.h>

/**
 * Test whether G(x -> Fy) is instantitated to G(a -> Fb)
 */
TEST(AtomicPropositionSubbingClonerTest, SimpleTest){

    //creating instantiating map
    std::map<std::string, std::string> map = std::map<std::string, std::string>();
    // fill map with x->a and y->b
    map.insert(std::pair<std::string,std::string>("x","a"));
    map.insert(std::pair<std::string,std::string>("y","b"));

    //parsing original formula...
    std::string input = "G(x -> Fy)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);
    ASSERT_EQ(input,spot::ltl::to_string(f));

    //checking ap subbing cloner
    const spot::ltl::formula* instantiatedf = texada::instantiate(f,map,std::vector<std::string>());
    ASSERT_EQ("G(a -> Fb)", to_string(instantiatedf));
    f->destroy();
    instantiatedf->destroy();


}

/**
 * Test whether G(x -> Fy) is instantitated to G(x -> Fb) when x is
 * specified as an event, not a variable
 */
TEST(AtomicPropositionSubbingClonerTest, SimpleConstantEventTest){
    //creating instantiating map
    std::map<std::string, std::string> map = std::map<std::string, std::string>();
    map.insert(std::pair<std::string,std::string>("y","b"));

    //parsing original formula...
    std::string input = "G(x -> Fy)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);
    ASSERT_EQ(input,spot::ltl::to_string(f));

    // add x to replace vector
    std::vector<std::string> dont_replace;
    dont_replace.push_back("x");

    //checking ap subbing cloner
    const spot::ltl::formula* instantiatedf = texada::instantiate(f,map,dont_replace);
    ASSERT_EQ("G(x -> Fb)", to_string(instantiatedf));
    f->destroy();
    //TODO: why is this causing a segfault?

    instantiatedf->destroy();


}

