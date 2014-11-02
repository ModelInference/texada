#include <gtest/gtest.h>
#include "../src/parsers/linearparser.h"
#include "../src/parsers/mapparser.h"
#include "../src/parsers/prefixtreeparser.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
// Tests that the linear parser correctly parses a small trace
TEST(SimpleParserTest, SmallFile) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/vary-tracelen/smalltrace.txt");

    texada::linear_parser * parser = new texada::linear_parser;
    parser->parse(infile);
    std::shared_ptr<std::set<std::vector<texada::event>>>trace_set =
    parser->return_vec_trace();
    std::shared_ptr<std::set<std::string>> events = parser->return_events();
    delete parser;
    parser = NULL;

    std::vector<texada::event> trace_vec = *trace_set->begin();
    ASSERT_TRUE(trace_vec[0].is_satisfied("e1"));
    ASSERT_TRUE(trace_vec[1].is_satisfied("e2"));
    ASSERT_TRUE(trace_vec[2].is_satisfied("e3"));
    ASSERT_TRUE(trace_vec[3].is_satisfied("e2"));
    ASSERT_TRUE(trace_vec[4].is_terminal());

    ASSERT_EQ(events->size(), 3);
    ASSERT_TRUE(events->find("e1") != events->end());
    ASSERT_TRUE(events->find("e2") != events->end());
    ASSERT_TRUE(events->find("e3") != events->end());

}

// Test that the linear parser separates multiple traces as it should
TEST(SimpleParserTest, MultipleTracesOneFile) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }

    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
    texada::linear_parser * parser = new texada::linear_parser;

    parser->parse(infile);
    std::shared_ptr<std::set<std::vector<texada::event>>>trace_set =
    parser->return_vec_trace();

    delete parser;
    parser = NULL;

    ASSERT_EQ(trace_set->size(),20)<< "Unexpected number of traces parsed";
    for (std::set<std::vector<texada::event> >::iterator it =
            trace_set->begin(); it != trace_set->end(); it++) {
        std::vector<texada::event> current_vector = *it;
        ASSERT_EQ(current_vector.size(), 251);
    }
}

// Checks that the map parser parses a small file properly
TEST(SimpleParserTest, MapTraceSmallFile) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }

    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/vary-tracelen/smalltrace.txt");
    texada::map_parser * parser = new texada::map_parser;
    parser->parse(infile);
    std::shared_ptr<
            std::set<std::map<texada::event, std::vector<long>>> >trace_set =
    parser->return_map_trace();
    delete parser;
    parser = NULL;

    ASSERT_EQ(trace_set->size(), 1);
    std::map<texada::event, std::vector<long>> trace =
            *trace_set->begin();
    ASSERT_EQ(trace.size(), 4);
    ASSERT_EQ(trace.at(texada::event("e1")).size(), 1);
    ASSERT_EQ(trace.at(texada::event("e2")).size(), 2);
    ASSERT_EQ(trace.at(texada::event("e3")).size(), 1);
    ASSERT_EQ(trace.at(texada::event()).size(), 1);
}

// checks that the map parser divides the traces properly, and
// that they're the right length (by checking the position of the terminal event)
TEST(SimpleParserTest, MapTraceLargeFile) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }

    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");

    texada::map_parser * parser = new texada::map_parser;
    parser->parse(infile);
    std::shared_ptr<
            std::set<std::map<texada::event, std::vector<long>>> >trace_set =
    parser->return_map_trace();
    delete parser;
    parser = NULL;

    ASSERT_EQ(trace_set->size(),20)<< "Unexpected number of traces parsed";
    for (std::set<std::map<texada::event, std::vector<long>> >::iterator it =
            trace_set->begin(); it != trace_set->end(); it++) {
        std::vector<long> end_vector = it->at(texada::event());
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

    texada::prefix_tree_parser parser;
    parser.parse(infile);
    std::shared_ptr<texada::prefix_tree> trace_set =
            parser.return_prefix_trees();

    ASSERT_TRUE(trace_set->get_trace_start(texada::event("a")) != NULL);
    ASSERT_TRUE(trace_set->get_trace_start(texada::event("a"))->get_child(texada::event("b")) != NULL);

    //trace1
    ASSERT_TRUE(trace_set->get_trace_start(1)->is_satisfied("a"));

    ASSERT_TRUE(trace_set->get_trace_start(1)->get_child(1)->is_satisfied("b"));
    ASSERT_TRUE(trace_set->get_trace_start(1)->get_child(1)->get_child(1)->is_satisfied("c"));
    ASSERT_TRUE(trace_set->get_trace_start(1)->get_child(1)->get_child(1)->get_child(
                    1)->is_satisfied("d"));

    //trace 2
    ASSERT_TRUE(trace_set->get_trace_start(2)->is_satisfied("a"));
    ASSERT_TRUE(trace_set->get_trace_start(2)->get_child(2)->is_satisfied("b"));

    ASSERT_TRUE(trace_set->get_trace_start(2)->get_child(2)->get_child(2)->is_satisfied("d"));
    ASSERT_TRUE(trace_set->get_trace_start(2)->get_child(2)->get_child(2)->get_child(
                    2)->is_satisfied("e"));
    //trace 3

    ASSERT_TRUE(trace_set->get_trace_start(3)->is_satisfied("a"));
    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->is_satisfied("c"));
    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->get_child(3)->is_satisfied("e"));
    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->get_child(3)->get_child(
                    3)->is_satisfied("e"));
    ASSERT_TRUE(trace_set->get_trace_start(3)->get_child(3)->get_child(3)->get_child(
                    3)->get_child(3)->is_satisfied("f"));

    //trace 4
    ASSERT_TRUE(trace_set->get_trace_start(4)->is_satisfied("a"));
    ASSERT_TRUE(trace_set->get_trace_start(4)->get_child(4)->is_satisfied("c"));
    ASSERT_TRUE(trace_set->get_trace_start(4)->get_child(4)->get_child(4)->is_satisfied("e"));
    ASSERT_TRUE(trace_set->get_trace_start(4)->get_child(4)->get_child(4)->get_child(
                    4)->is_satisfied("d"));
    ASSERT_TRUE(trace_set->get_trace_start(4)->get_child(4)->get_child(4)->get_child(
                    4)->get_child(4)->is_satisfied("EndOfTraceVar"));


}

// Checks that the parser can be configured for custom separator line
TEST(SimpleParserTest, CustomSeparator) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }

    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/regex-parsing-tests/custom-separator.txt");
    texada::linear_parser * parser = new texada::linear_parser;
    parser->set_trace_separator("break");

    parser->parse(infile);
    std::shared_ptr<std::set<std::vector<texada::event>>>trace_set =
    parser->return_vec_trace();

    delete parser;
    parser = NULL;

    ASSERT_EQ(trace_set->size(),20)<< "Unexpected number of traces parsed";
    for (std::set<std::vector<texada::event> >::iterator it =
            trace_set->begin(); it != trace_set->end(); it++) {
        std::vector<texada::event> current_vector = *it;
        ASSERT_EQ(current_vector.size(), 251);
    }
}

// Checks that the parser can be configured to recognize event types using the regex option
TEST(SimpleParserTest, RegexOption) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/regex-parsing-tests/small-structured-trace.txt");

    texada::linear_parser * parser = new texada::linear_parser;
    std::vector<std::string> etypes;
    etypes.push_back("(?<USER>.*)(?<ETYPE>e[0-9])");
    parser->set_event_types(etypes);

    parser->parse(infile);
    std::shared_ptr<std::set<std::vector<texada::event>>>trace_set =
    parser->return_vec_trace();
    std::shared_ptr<std::set<std::string>> events = parser->return_events();
    delete parser;
    parser = NULL;

    std::vector<texada::event> trace_vec = *trace_set->begin();
    ASSERT_TRUE(trace_vec[0].is_satisfied("e1"));
    ASSERT_TRUE(trace_vec[1].is_satisfied("e2"));
    ASSERT_TRUE(trace_vec[2].is_satisfied("e3"));
    ASSERT_TRUE(trace_vec[3].is_satisfied("e2"));
    ASSERT_TRUE(trace_vec[4].is_terminal());

    ASSERT_EQ(events->size(), 3);
    ASSERT_TRUE(events->find("e1") != events->end());
    ASSERT_TRUE(events->find("e2") != events->end());
    ASSERT_TRUE(events->find("e3") != events->end());
}

// Checks that the parser can be configured using multiple regular expressions
TEST(SimpleParserTest, MultipleRegexOptions) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/regex-parsing-tests/small-structured-trace2.txt");

    texada::linear_parser * parser = new texada::linear_parser;
    std::vector<std::string> etypes;
    etypes.push_back("--> (?<ETYPE>e[0-9])");
    etypes.push_back("<-- (?<ETYPE>e[0-9])");
    parser->set_event_types(etypes);

    parser->parse(infile);
    std::shared_ptr<std::set<std::vector<texada::event>>>trace_set =
    parser->return_vec_trace();
    std::shared_ptr<std::set<std::string>> events = parser->return_events();
    delete parser;
    parser = NULL;

    std::vector<texada::event> trace_vec = *trace_set->begin();
    ASSERT_TRUE(trace_vec[0].is_satisfied("e1"));
    ASSERT_TRUE(trace_vec[1].is_satisfied("e2"));
    ASSERT_TRUE(trace_vec[2].is_satisfied("e3"));
    ASSERT_TRUE(trace_vec[3].is_satisfied("e2"));
    ASSERT_TRUE(trace_vec[4].is_terminal());

    ASSERT_EQ(events->size(), 3);
    ASSERT_TRUE(events->find("e1") != events->end());
    ASSERT_TRUE(events->find("e2") != events->end());
    ASSERT_TRUE(events->find("e3") != events->end());
}

// Checks that the parser can be configured to ignore non matching lines
TEST(SimpleParserTest, IgnoreNMLines) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/regex-parsing-tests/ignore-nm-lines-trace.txt");

    texada::linear_parser * parser = new texada::linear_parser;
    parser->ignore_nm_lines();
    std::vector<std::string> etypes;
    etypes.push_back("--> (?<ETYPE>e[0-9])");
    parser->set_event_types(etypes);

    parser->parse(infile);
    std::shared_ptr<std::set<std::vector<texada::event>>>trace_set =
    parser->return_vec_trace();
    std::shared_ptr<std::set<std::string>> events = parser->return_events();
    delete parser;
    parser = NULL;

    std::vector<texada::event> trace_vec = *trace_set->begin();
    ASSERT_TRUE(trace_vec[0].is_satisfied("e1"));
    ASSERT_TRUE(trace_vec[1].is_satisfied("e2"));
    ASSERT_TRUE(trace_vec[2].is_satisfied("e3"));
    ASSERT_TRUE(trace_vec[3].is_satisfied("e2"));
    ASSERT_TRUE(trace_vec[4].is_terminal());

    ASSERT_EQ(events->size(), 3);
    ASSERT_TRUE(events->find("e1") != events->end());
    ASSERT_TRUE(events->find("e2") != events->end());
    ASSERT_TRUE(events->find("e3") != events->end());
}
