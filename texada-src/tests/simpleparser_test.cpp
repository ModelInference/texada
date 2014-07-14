#include <gtest/gtest.h>
#include "../src/parsing/simpleparser.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
// Tests that the simple parser correctly parses a small trace
TEST(SimpleParserTest, SmallFile) {

    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/vary-tracelen/smalltrace.txt");

    texada::simple_parser * parser = new texada::simple_parser;
    parser->parse_to_vector(infile);
    std::set<std::vector<texada::string_event> > trace_set =
            parser->return_vec_trace();
    std::set<std::string> events = parser->return_events();
    delete parser;
    parser = NULL;

    std::vector<texada::string_event> trace_vec = *trace_set.begin();
    ASSERT_EQ("e1", trace_vec[0].get_name());
    ASSERT_EQ("e2", trace_vec[1].get_name());
    ASSERT_EQ("e3", trace_vec[2].get_name());
    ASSERT_EQ("e2", trace_vec[3].get_name());
    ASSERT_TRUE(trace_vec[4].is_terminal());

    ASSERT_EQ(events.size(), 3);
    ASSERT_TRUE(events.find("e1") != events.end());
    ASSERT_TRUE(events.find("e2") != events.end());
    ASSERT_TRUE(events.find("e3") != events.end());

}

// Test that the parser separates multiple traces as it should
TEST(SimpleParserTest, MultipleTracesOneFile) {
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
    texada::simple_parser * parser = new texada::simple_parser;

    parser->parse_to_vector(infile);
    std::set<std::vector<texada::string_event> > trace_set =
            parser->return_vec_trace();

    delete parser;
    parser = NULL;

    ASSERT_EQ(trace_set.size(),20)<< "Unexpected number of traces parsed";
    for (std::set<std::vector<texada::string_event> >::iterator it =
            trace_set.begin(); it != trace_set.end(); it++) {
        std::vector<texada::string_event> current_vector = *it;
        ASSERT_EQ(current_vector.size(), 251);
    }
}

// Checks that the parser parses a small file into a map properly
TEST(SimpleParserTest, MapTraceSmallFile) {
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/vary-tracelen/smalltrace.txt");
    texada::simple_parser * parser = new texada::simple_parser;
    parser->parse_to_map(infile);
    std::set<std::map<texada::string_event, std::vector<long>> > trace_set =
            parser->return_map_trace();
    delete parser;
    parser = NULL;

    ASSERT_EQ(trace_set.size(), 1);
    std::map<texada::string_event, std::vector<long>> trace =
            *trace_set.begin();
    ASSERT_EQ(trace.size(), 4);
    ASSERT_EQ(trace.at(texada::string_event("e1")).size(), 1);
    ASSERT_EQ(trace.at(texada::string_event("e2")).size(), 2);
    ASSERT_EQ(trace.at(texada::string_event("e3")).size(), 1);
    ASSERT_EQ(trace.at(texada::string_event()).size(), 1);
}

// checks that the parser divides the map traces properly, and
// that they're the right length (by checking the position of the terminal event)
TEST(SimpleParserTest, MapTraceLargeFile) {
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");

    texada::simple_parser * parser = new texada::simple_parser;
    parser->parse_to_map(infile);
    std::set<std::map<texada::string_event, std::vector<long>> > trace_set =
            parser->return_map_trace();
    delete parser;
    parser = NULL;

    ASSERT_EQ(trace_set.size(),20)<< "Unexpected number of traces parsed";
    for (std::set<std::map<texada::string_event, std::vector<long>> >::iterator it =
            trace_set.begin(); it != trace_set.end(); it++) {
        std::vector<long> end_vector = it->at(texada::string_event());
        ASSERT_EQ(end_vector[0], 250);
    }

}
