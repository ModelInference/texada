/*
 * multipropparser_test.cpp
 *
 *  Created on: Nov 1, 2014
 *      Author: dennis
 */

#include <gtest/gtest.h>
#include "../src/parsers/linearparser.h"
#include "../src/parsers/mapparser.h"
#include "../src/parsers/prefixtreeparser.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>

// Tests that the linear parser correctly parses a small multi-propositional log
TEST(MultiPropParserTest, LinearTest) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/mult-prop-tests/mplog.txt");

    texada::linear_parser * parser = new texada::linear_parser;
    parser->set_event_separator("==");
    parser->parse(infile);
    std::shared_ptr<std::multiset<std::vector<texada::event>>>trace_set =
    parser->return_vec_trace();
    std::shared_ptr<std::set<std::string>> events = parser->return_events();
    delete parser;
    parser = NULL;

    std::vector<texada::event> trace_vec = *trace_set->begin();
    ASSERT_TRUE(trace_vec[0].is_satisfied("a"));
    ASSERT_TRUE(trace_vec[0].is_satisfied("b"));
    ASSERT_FALSE(trace_vec[0].is_satisfied("c"));
    ASSERT_FALSE(trace_vec[0].is_satisfied("d"));
    ASSERT_FALSE(trace_vec[0].is_satisfied("e"));

    ASSERT_TRUE(trace_vec[1].is_satisfied("a"));
    ASSERT_TRUE(trace_vec[1].is_satisfied("b"));
    ASSERT_FALSE(trace_vec[1].is_satisfied("c"));
    ASSERT_TRUE(trace_vec[1].is_satisfied("d"));
    ASSERT_FALSE(trace_vec[1].is_satisfied("e"));

    ASSERT_FALSE(trace_vec[2].is_satisfied("a"));
    ASSERT_FALSE(trace_vec[2].is_satisfied("b"));
    ASSERT_TRUE(trace_vec[2].is_satisfied("c"));
    ASSERT_FALSE(trace_vec[2].is_satisfied("d"));
    ASSERT_FALSE(trace_vec[2].is_satisfied("e"));

    ASSERT_FALSE(trace_vec[3].is_satisfied("a"));
    ASSERT_TRUE(trace_vec[3].is_satisfied("b"));
    ASSERT_FALSE(trace_vec[3].is_satisfied("c"));
    ASSERT_FALSE(trace_vec[3].is_satisfied("d"));
    ASSERT_TRUE(trace_vec[3].is_satisfied("e"));

    ASSERT_TRUE(trace_vec[4].is_terminal());

    ASSERT_EQ(events->size(), 5);
    ASSERT_TRUE(events->find("a") != events->end());
    ASSERT_TRUE(events->find("b") != events->end());
    ASSERT_TRUE(events->find("c") != events->end());
    ASSERT_TRUE(events->find("d") != events->end());
    ASSERT_TRUE(events->find("e") != events->end());
}

// Tests that the map parser correctly parses a small multi-propositional log
TEST(MultiPropParserTest, MapTest) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/mult-prop-tests/mplog.txt");

    texada::map_parser * parser = new texada::map_parser;
    parser->set_event_separator("==");
    parser->parse(infile);
    std::shared_ptr<
            std::set<std::map<texada::event, std::vector<long>>> >trace_set =
    parser->return_map_trace();
    delete parser;
    parser = NULL;

    ASSERT_EQ(trace_set->size(), 1);
    std::map<texada::event, std::vector<long>> trace =
            *trace_set->begin();
    ASSERT_EQ(trace.size(), 5);
    std::set<std::string> props0 = {"a", "b"};
    std::set<std::string> props1 = {"a", "b", "d"};
    std::set<std::string> props2 = {"c"};
    std::set<std::string> props3 = {"b", "e"};
    ASSERT_EQ(trace.at(texada::event(props0)).size(), 1);
    ASSERT_EQ(trace.at(texada::event(props1)).size(), 1);
    ASSERT_EQ(trace.at(texada::event(props2)).size(), 1);
    ASSERT_EQ(trace.at(texada::event(props3)).size(), 1);
    ASSERT_EQ(trace.at(texada::event()).size(), 1);
}

// Tests that the prefixtree parser correctly parses a small multi-propositional log
TEST(MultiPropParserTest, PrefixTest) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }

    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/mult-prop-tests/multitrace-mplog.txt");

    texada::prefix_tree_parser parser;
    parser.set_event_separator("==");
    parser.parse(infile);
    std::shared_ptr<texada::prefix_tree> trace_set =
            parser.return_prefix_trees();

    //trace1
    ASSERT_TRUE(trace_set->get_trace_start(1)->is_satisfied("a"));
    ASSERT_TRUE(trace_set->get_trace_start(1)->is_satisfied("b"));
    ASSERT_FALSE(trace_set->get_trace_start(1)->is_satisfied("c"));
    ASSERT_FALSE(trace_set->get_trace_start(1)->is_satisfied("d"));

    ASSERT_FALSE(trace_set->get_trace_start(1)->get_child(1)->is_satisfied("a"));
    ASSERT_FALSE(trace_set->get_trace_start(1)->get_child(1)->is_satisfied("b"));
    ASSERT_TRUE(trace_set->get_trace_start(1)->get_child(1)->is_satisfied("c"));
    ASSERT_TRUE(trace_set->get_trace_start(1)->get_child(1)->is_satisfied("d"));

    ASSERT_TRUE(trace_set->get_trace_start(1)->get_child(1)->get_child(1)->is_satisfied("a"));
    ASSERT_FALSE(trace_set->get_trace_start(1)->get_child(1)->get_child(1)->is_satisfied("b"));
    ASSERT_FALSE(trace_set->get_trace_start(1)->get_child(1)->get_child(1)->is_satisfied("c"));
    ASSERT_TRUE(trace_set->get_trace_start(1)->get_child(1)->get_child(1)->is_satisfied("d"));

    ASSERT_FALSE(trace_set->get_trace_start(1)->get_child(1)->get_child(1)->get_child(1)->is_satisfied("a"));
    ASSERT_FALSE(trace_set->get_trace_start(1)->get_child(1)->get_child(1)->get_child(1)->is_satisfied("b"));
    ASSERT_TRUE(trace_set->get_trace_start(1)->get_child(1)->get_child(1)->get_child(1)->is_satisfied("c"));
    ASSERT_FALSE(trace_set->get_trace_start(1)->get_child(1)->get_child(1)->get_child(1)->is_satisfied("d"));

    ASSERT_TRUE(trace_set->get_trace_start(1)->get_child(1)->get_child(1)->get_child(1)->get_child(1)->is_satisfied("EndOfTraceVar"));

    //trace 2
    ASSERT_TRUE(trace_set->get_trace_start(2)->is_satisfied("a"));
    ASSERT_TRUE(trace_set->get_trace_start(2)->is_satisfied("b"));
    ASSERT_FALSE(trace_set->get_trace_start(2)->is_satisfied("c"));
    ASSERT_FALSE(trace_set->get_trace_start(2)->is_satisfied("d"));

    ASSERT_FALSE(trace_set->get_trace_start(2)->get_child(2)->is_satisfied("a"));
    ASSERT_FALSE(trace_set->get_trace_start(2)->get_child(2)->is_satisfied("b"));
    ASSERT_FALSE(trace_set->get_trace_start(2)->get_child(2)->is_satisfied("c"));
    ASSERT_TRUE(trace_set->get_trace_start(2)->get_child(2)->is_satisfied("d"));

    ASSERT_TRUE(trace_set->get_trace_start(2)->get_child(2)->get_child(2)->is_satisfied("a"));
    ASSERT_FALSE(trace_set->get_trace_start(2)->get_child(2)->get_child(2)->is_satisfied("b"));
    ASSERT_FALSE(trace_set->get_trace_start(2)->get_child(2)->get_child(2)->is_satisfied("c"));
    ASSERT_TRUE(trace_set->get_trace_start(2)->get_child(2)->get_child(2)->is_satisfied("d"));

    ASSERT_TRUE(trace_set->get_trace_start(2)->get_child(2)->get_child(2)->get_child(2)->is_satisfied("a"));
    ASSERT_TRUE(trace_set->get_trace_start(2)->get_child(2)->get_child(2)->get_child(2)->is_satisfied("b"));
    ASSERT_FALSE(trace_set->get_trace_start(2)->get_child(2)->get_child(2)->get_child(2)->is_satisfied("c"));
    ASSERT_FALSE(trace_set->get_trace_start(2)->get_child(2)->get_child(2)->get_child(2)->is_satisfied("d"));

    ASSERT_TRUE(trace_set->get_trace_start(2)->get_child(2)->get_child(2)->get_child(2)->get_child(2)->is_satisfied("EndOfTraceVar"));

    //trace 3
    ASSERT_TRUE(trace_set->get_trace_start(3)->is_satisfied("a"));
    ASSERT_TRUE(trace_set->get_trace_start(3)->is_satisfied("b"));
    ASSERT_FALSE(trace_set->get_trace_start(3)->is_satisfied("c"));
    ASSERT_FALSE(trace_set->get_trace_start(3)->is_satisfied("d"));

    ASSERT_FALSE(trace_set->get_trace_start(3)->get_child(3)->is_satisfied("a"));
    ASSERT_FALSE(trace_set->get_trace_start(3)->get_child(3)->is_satisfied("b"));
    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->is_satisfied("c"));
    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->is_satisfied("d"));

    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->get_child(3)->is_satisfied("a"));
    ASSERT_FALSE(trace_set->get_trace_start(3)->get_child(3)->get_child(3)->is_satisfied("b"));
    ASSERT_FALSE(trace_set->get_trace_start(3)->get_child(3)->get_child(3)->is_satisfied("c"));
    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->get_child(3)->is_satisfied("d"));

    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->get_child(3)->get_child(3)->is_satisfied("a"));
    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->get_child(3)->get_child(3)->is_satisfied("b"));
    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->get_child(3)->get_child(3)->is_satisfied("c"));
    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->get_child(3)->get_child(3)->is_satisfied("d"));

    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->get_child(3)->get_child(3)->get_child(3)->is_satisfied("EndOfTraceVar"));
}

