/*
 * instantspoolcreator_test.cpp
 *
 *  Created on: May 21, 2014
 *      Author: clemieux
 */

#include "../src/instantiation-tools/pregeninstantspool.h"
#include <gtest/gtest.h>
#include "../src/instantiation-tools/texadatospotmapping.h"

/**
 * Tests the creation of the pool of instantiation functions
 * by checking if the returned array contains all 2-event
 * permutations with repetition of a,b,c.
 */

TEST(InstantiatorPoolCreatorTest,TwoBindingsThreeEvents) {

    //Set up to get the atomic propositions from a property type
    std::string input = "G(x -> Fy)";
    texada::ltl::parse_error_list pel;
    const texada::ltl::formula* f = texada::ltl::parse(input, pel);
    std::shared_ptr<texada::ltl::atomic_prop_set> formula_vars(texada::ltl::atomic_prop_collect(
            f));
    // create a set of events
    std::shared_ptr<std::set<std::string>> events = std::make_shared<
            std::set<std::string>>();
    events->insert("a");
    events->insert("b");
    events->insert("c");
    // now create the event instantiator

    texada::pregen_instants_pool instantiator = texada::pregen_instants_pool(
            events, formula_vars, true,std::vector<std::string>());

    std::shared_ptr<std::vector<std::map<std::string, std::string>>>returned_array = instantiator.return_instantiations();
    //check that all expected mappings occur

    ASSERT_EQ(returned_array->at(0).at("x"), "a");
    ASSERT_EQ(returned_array->at(0).at("y"), "a");
    ASSERT_EQ(returned_array->at(1).at("x"), "b");
    ASSERT_EQ(returned_array->at(1).at("y"), "a");
    ASSERT_EQ(returned_array->at(2).at("x"), "c");
    ASSERT_EQ(returned_array->at(2).at("y"), "a");
    ASSERT_EQ(returned_array->at(3).at("x"), "a");
    ASSERT_EQ(returned_array->at(3).at("y"), "b");
    ASSERT_EQ(returned_array->at(4).at("x"), "b");
    ASSERT_EQ(returned_array->at(4).at("y"), "b");
    ASSERT_EQ(returned_array->at(5).at("x"), "c");
    ASSERT_EQ(returned_array->at(5).at("y"), "b");
    ASSERT_EQ(returned_array->at(6).at("x"), "a");
    ASSERT_EQ(returned_array->at(6).at("y"), "c");
    ASSERT_EQ(returned_array->at(7).at("x"), "b");
    ASSERT_EQ(returned_array->at(7).at("y"), "c");
    ASSERT_EQ(returned_array->at(8).at("x"), "c");
    ASSERT_EQ(returned_array->at(8).at("y"), "c");

    ASSERT_EQ(returned_array->at(0), *instantiator.get_next_instantiation());
    f->destroy();

}

TEST(InstantiatorPoolCreatorTest,CheckNoRepetition) {
    //Set up to get the atomic propositions from a property type
    std::string input = "G(x -> Fy)";
    texada::ltl::parse_error_list pel;
    const texada::ltl::formula* f = texada::ltl::parse(input, pel);
    std::shared_ptr<texada::ltl::atomic_prop_set> formula_vars (texada::ltl::atomic_prop_collect(
            f));
    // create a set of events
    std::shared_ptr<std::set<std::string>> events = std::make_shared<
            std::set<std::string>>();
    events->insert("a");
    events->insert("b");
    events->insert("c");
    // now create the event instantiator

    texada::pregen_instants_pool instantiator = texada::pregen_instants_pool(
            events, formula_vars, false, std::vector<std::string>());

    std::shared_ptr<std::vector<std::map<std::string, std::string>>>returned_array = instantiator.return_instantiations();
    //check that all expected mappings occur

    ASSERT_EQ(returned_array->at(1), *instantiator.get_next_instantiation());
    ASSERT_EQ(returned_array->at(2), *instantiator.get_next_instantiation());
    ASSERT_EQ(returned_array->at(3), *instantiator.get_next_instantiation());
    ASSERT_EQ(returned_array->at(5), *instantiator.get_next_instantiation());
    ASSERT_EQ(returned_array->at(6), *instantiator.get_next_instantiation());
    ASSERT_EQ(returned_array->at(7), *instantiator.get_next_instantiation());
    ASSERT_TRUE(instantiator.get_next_instantiation() == NULL);

    f->destroy();

}

