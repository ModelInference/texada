/*
 * truncatingchecker_test.cpp
 *
 *  Created on: Jul 15, 2014
 *      Author: clemieux
 */

#include "../src/checkers/truncatingchecker.h"
#include <gtest/gtest.h>
#include <ltlparse/public.hh>

TEST(TruncatingCheckerTest,Simple) {

    //create a set of traces and add them
    std::vector<texada::string_event> trace_vec;
    std::set<std::vector<texada::string_event>> trace_set;

    // create trace a a b b c
    trace_vec.push_back(texada::string_event("a"));
    trace_vec.push_back(texada::string_event("a"));
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event("c"));
    trace_vec.push_back(texada::string_event());
    trace_set.insert(trace_vec);

    // create trace b a b c
    trace_vec.clear();
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event("a"));
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event("c"));
    trace_vec.push_back(texada::string_event());
    trace_set.insert(trace_vec);

    // create trace c b
    trace_vec.clear();
    trace_vec.push_back(texada::string_event("c"));
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event());
    trace_set.insert(trace_vec);

    // parse the formula
    std::string input = "G(x->XFy)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* afby_form = spot::ltl::parse(input, pel);

    //create event set
    std::set<std::string> unique_events;
    unique_events.insert("a");
    unique_events.insert("b");
    unique_events.insert("c");

    //really the checker is more of a miner because it does instantiation by itself...

    texada::truncating_checker checker = texada::truncating_checker(afby_form,
            unique_events);

    std::vector<std::map<std::string, std::string>> valid_instants =
            checker.return_valid_instants(afby_form, trace_set);

    // should contain a afby b and a afby c

    ASSERT_EQ(2,valid_instants.size());
}

