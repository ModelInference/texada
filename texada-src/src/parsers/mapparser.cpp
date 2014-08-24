/*
 * mapparser.cpp
 *
 *  Created on: Aug 19, 2014
 *      Author: dennis
 */

#include "mapparser.h"

namespace texada {

map_parser::map_parser() {
    pos_count = 0;
    temp_trace.clear();
    map_trace_set = std::make_shared<set<map<string_event, vector<long>>> >();
}

map_parser::~map_parser() {
}

/**
 * Returns the set of parsed map traces
 * @return
 */
shared_ptr<set<map<string_event, vector<long>>> > map_parser::return_map_trace() {
    if (has_been_parsed) {
        return map_trace_set;
    } else {
        std::cerr << "Trace not parsed into map, returning empty set. \n";
        return map_trace_set;
    }
}

/**
 * Helper function called from within parser::parse(std::ifstream &infile)
 * Ends the current trace being built
 */
void map_parser::end_trace() {
    std::vector<long> pos_vec;
    pos_vec.push_back(pos_count);
    temp_trace.emplace(string_event(), pos_vec);
    map_trace_set->insert(temp_trace);
    temp_trace.clear();
    pos_count = 0;
}

/**
 * Helper function called from within parser::parse(std::ifstream &infile)
 * Adds event to the current trace being built
 * @param event name
 */
void map_parser::add_event(std::string event) {
    if ((temp_trace.find(string_event(event)) == temp_trace.end())) {
        std::vector<long> pos_vec;
        pos_vec.push_back(pos_count);
        temp_trace.emplace(string_event(event), pos_vec);
        pos_count++;
    } else {
        temp_trace.at(string_event(event)).push_back(pos_count);
        pos_count++;
    }
}

} /* namespace texada */
