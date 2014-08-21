/*
 * linearparser.cpp
 *
 *  Created on: Aug 19, 2014
 *      Author: dennis
 */

#include "linearparser.h"
#include <vector>

namespace texada {

linear_parser::linear_parser() {
    temp_trace.clear();
    vector_trace_set = std::make_shared<set<vector<string_event>>>();
}

linear_parser::~linear_parser() {
}

/**
 * Returns the set of parsed vector traces
 * @return
 */
shared_ptr<set<vector<string_event> >> linear_parser::return_vec_trace() {
    if (has_been_parsed) {
        return vector_trace_set;
    } else {
        std::cerr << "Trace not parsed into vector, returning empty set. \n";
        return vector_trace_set;
    }

}

/**
 * Helper function called from within parser::parse(std::ifstream &infile)
 * Ends the current trace being built
 */
void linear_parser::end_trace() {
    temp_trace.push_back(string_event());
    vector_trace_set->insert(temp_trace);
    temp_trace.clear();
}

/**
 * Helper function called from within parser::parse(std::ifstream &infile)
 * Adds event to the current trace being built
 * @param event name
 */
void linear_parser::add_event(std::string event) {
    temp_trace.push_back(string_event(event));
}

} /* namespace texada */
