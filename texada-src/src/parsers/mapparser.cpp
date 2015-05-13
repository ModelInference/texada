/*
 * mapparser.cpp
 *
 *  Created on: Aug 19, 2014
 *      Author: dennis
 */

#include "mapparser.h"
#include "../trace/event.h"

namespace texada {

map_parser::map_parser() {
    pos_count = 0;
    temp_trace.clear();
    map_trace_set = std::make_shared<set<map<event, vector<long>>> >();
}

map_parser::~map_parser() {
}

/**
 * Returns the set of parsed map traces
 * @return
 */
shared_ptr<set<map<event, vector<long>>> > map_parser::return_map_trace() {
    if (has_been_parsed) {
        return map_trace_set;
    } else {
        std::cerr << "Trace not parsed into map, returning empty set. \n";
        return map_trace_set;
    }
}

/**
 * Helper function called from within parser::parse(std::ifstream &infile)
 * Adds event to the current trace being built
 * @param event new event
 */
void map_parser::add_event(event event_parent) {
    // if event_parent is terminal, add and return
    if (event_parent.is_terminal()) {
        std::vector<long> pos_vec;
        pos_vec.push_back(pos_count);
        temp_trace.emplace(event_parent, pos_vec);
        map_trace_set->insert(temp_trace);
        temp_trace.clear();
        pos_count = 0;
        return;
    }
    // else add each atomic proposition to an individual map
    std::set<std::string> set_of_event_names = event_parent.get_props();
    for (std::set<std::string>::iterator it = set_of_event_names.begin(); it != set_of_event_names.end(); it++){
        event event_ = event(*it);
        parser::add_event(event_);
        if ((temp_trace.find(event_) == temp_trace.end())) {
          std::vector<long> pos_vec;
          pos_vec.push_back(pos_count);
          temp_trace.emplace(event_, pos_vec);
        } else {
          temp_trace.at(event_).push_back(pos_count);
        }
    }
   // increment position only after all atomic props have been added
   pos_count++;
}

} /* namespace texada */
