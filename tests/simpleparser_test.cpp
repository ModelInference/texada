#include <gtest/gtest.h>
#include "../src/simpleparser.h"
#include <fstream>
#include <iostream>


TEST(SimpleParserTest, SmallFile){
	std::ifstream infile("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/smalltrace.txt");
	texada::simple_parser * parser = new texada::simple_parser();
	std::set<std::vector<texada::string_event> > * trace_set = parser->parse(infile);
	std::vector<texada::string_event> trace_vec = *trace_set->begin();
    ASSERT_EQ("e1", trace_vec[0].get_name());
	ASSERT_EQ("e2", trace_vec[1].get_name());
	ASSERT_EQ("e3", trace_vec[2].get_name());
	ASSERT_EQ("e2", trace_vec[3].get_name());
	ASSERT_TRUE(trace_vec[4].is_terminal());
	std::set<std::string> * events = parser->return_events();
	ASSERT_EQ(events->size(),3);
	ASSERT_TRUE(events->find("e1")!=events->end());
	ASSERT_TRUE(events->find("e2")!=events->end());
	ASSERT_TRUE(events->find("e3")!=events->end());

}
TEST(SimpleParserTest, MultipleTracesOneFile){
	std::ifstream infile("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
	texada::simple_parser * parser = new texada::simple_parser();
	std::set<std::vector<texada::string_event> > * trace_set = parser->parse(infile);
	ASSERT_EQ(trace_set->size(),20) << "Unexpected number of traces parsed";
	for(std::set<std::vector<texada::string_event> >::iterator it =trace_set->begin();
			it != trace_set->end(); it++){
		std::vector<texada::string_event> current_vector = *it;
		ASSERT_EQ(current_vector.size(), 251);
	}

}
