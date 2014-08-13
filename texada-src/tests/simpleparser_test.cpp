#include <gtest/gtest.h>
#include "../src/parsing/simpleparser.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
// Tests that the simple parser correctly parses a small trace
TEST(SimpleParserTest, SmallFile) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/vary-tracelen/smalltrace.txt");

    texada::simple_parser * parser = new texada::simple_parser;
    parser->parse_to_vector(infile);
    std::shared_ptr<std::set<std::vector<texada::string_event>>>trace_set =
    parser->return_vec_trace();
    std::shared_ptr<std::set<std::string>> events = parser->return_events();
    delete parser;
    parser = NULL;

    std::vector<texada::string_event> trace_vec = *trace_set->begin();
    ASSERT_EQ("e1", trace_vec[0].get_name());
    ASSERT_EQ("e2", trace_vec[1].get_name());
    ASSERT_EQ("e3", trace_vec[2].get_name());
    ASSERT_EQ("e2", trace_vec[3].get_name());
    ASSERT_TRUE(trace_vec[4].is_terminal());

    ASSERT_EQ(events->size(), 3);
    ASSERT_TRUE(events->find("e1") != events->end());
    ASSERT_TRUE(events->find("e2") != events->end());
    ASSERT_TRUE(events->find("e3") != events->end());

}

// Test that the parser separates multiple traces as it should
TEST(SimpleParserTest, MultipleTracesOneFile) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }

    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
    texada::simple_parser * parser = new texada::simple_parser;

    parser->parse_to_vector(infile);
    std::shared_ptr<std::set<std::vector<texada::string_event>>>trace_set =
    parser->return_vec_trace();

    delete parser;
    parser = NULL;

    ASSERT_EQ(trace_set->size(),20)<< "Unexpected number of traces parsed";
    for (std::set<std::vector<texada::string_event> >::iterator it =
            trace_set->begin(); it != trace_set->end(); it++) {
        std::vector<texada::string_event> current_vector = *it;
        ASSERT_EQ(current_vector.size(), 251);
    }
}

// Checks that the parser parses a small file into a map properly
TEST(SimpleParserTest, MapTraceSmallFile) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }

    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/vary-tracelen/smalltrace.txt");
    texada::simple_parser * parser = new texada::simple_parser;
    parser->parse_to_map(infile);
    std::shared_ptr<
            std::set<std::map<texada::string_event, std::vector<long>>> >trace_set =
    parser->return_map_trace();
    delete parser;
    parser = NULL;

    ASSERT_EQ(trace_set->size(), 1);
    std::map<texada::string_event, std::vector<long>> trace =
            *trace_set->begin();
    ASSERT_EQ(trace.size(), 4);
    ASSERT_EQ(trace.at(texada::string_event("e1")).size(), 1);
    ASSERT_EQ(trace.at(texada::string_event("e2")).size(), 2);
    ASSERT_EQ(trace.at(texada::string_event("e3")).size(), 1);
    ASSERT_EQ(trace.at(texada::string_event()).size(), 1);
}

// checks that the parser divides the map traces properly, and
// that they're the right length (by checking the position of the terminal event)
TEST(SimpleParserTest, MapTraceLargeFile) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }

    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");

    texada::simple_parser * parser = new texada::simple_parser;
    parser->parse_to_map(infile);
    std::shared_ptr<
            std::set<std::map<texada::string_event, std::vector<long>>> >trace_set =
    parser->return_map_trace();
    delete parser;
    parser = NULL;

    ASSERT_EQ(trace_set->size(),20)<< "Unexpected number of traces parsed";
    for (std::set<std::map<texada::string_event, std::vector<long>> >::iterator it =
            trace_set->begin(); it != trace_set->end(); it++) {
        std::vector<long> end_vector = it->at(texada::string_event());
        ASSERT_EQ(end_vector[0], 250);
    }

}

TEST(SimpleParserTest, PreTreeTrace) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }

    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/parsing-tests/simple-pre-tree.txt");

    texada::simple_parser parser;
    parser.parse_to_pretrees(infile);
    std::shared_ptr<texada::prefix_tree> trace_set =
            parser.return_prefix_trees();

    ASSERT_TRUE(trace_set->get_trace_start("a") != NULL);
    ASSERT_TRUE(trace_set->get_trace_start("a")->get_child("b") != NULL);

    //trace1
    ASSERT_EQ("a", trace_set->get_trace_start(1)->get_name());

    ASSERT_EQ("b", trace_set->get_trace_start(1)->get_child(1)->get_name());
    ASSERT_EQ("c",
            trace_set->get_trace_start(1)->get_child(1)->get_child(1)->get_name());
    ASSERT_EQ("d",
            trace_set->get_trace_start(1)->get_child(1)->get_child(1)->get_child(
                    1)->get_name());

    //trace 2
    ASSERT_EQ("a", trace_set->get_trace_start(2)->get_name());
    ASSERT_EQ("b", trace_set->get_trace_start(2)->get_child(2)->get_name());

    ASSERT_EQ("d",
            trace_set->get_trace_start(2)->get_child(2)->get_child(2)->get_name());
    ASSERT_EQ("e",
            trace_set->get_trace_start(2)->get_child(2)->get_child(2)->get_child(
                    2)->get_name());
    //trace 3

    ASSERT_EQ("a", trace_set->get_trace_start(3)->get_name());
    ASSERT_EQ("c", trace_set->get_trace_start(3)->get_child(3)->get_name());
    ASSERT_EQ("e",
            trace_set->get_trace_start(3)->get_child(3)->get_child(3)->get_name());
    ASSERT_EQ("e",
            trace_set->get_trace_start(3)->get_child(3)->get_child(3)->get_child(
                    3)->get_name());
    ASSERT_EQ("f",
            trace_set->get_trace_start(3)->get_child(3)->get_child(3)->get_child(
                    3)->get_child(3)->get_name());

    //trace 4
    ASSERT_EQ("a", trace_set->get_trace_start(4)->get_name());
    ASSERT_EQ("c", trace_set->get_trace_start(4)->get_child(4)->get_name());
    ASSERT_EQ("e",
            trace_set->get_trace_start(4)->get_child(4)->get_child(4)->get_name());
    ASSERT_EQ("d",
            trace_set->get_trace_start(4)->get_child(4)->get_child(4)->get_child(
                    4)->get_name());
    ASSERT_EQ("EndOfTraceVar",
            trace_set->get_trace_start(4)->get_child(4)->get_child(4)->get_child(
                    4)->get_child(4)->get_name());


}
