/*
 * simpleparser.cpp
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#include "simpleparser.h"
#include <vector>
#include <fstream>
#include <iostream>

namespace texada {

simple_parser::simple_parser() {
    unique_events = std::make_shared<set<string>>();
    vector_trace_set = std::make_shared<set<vector<string_event>>>();
    map_trace_set = std::make_shared<set<map<string_event, vector<long>>> >();
    pre_tree_traces = std::make_shared<prefix_tree>();
}

simple_parser::~simple_parser() {
}

/**
 * Parses the given file into a vector, fills the event set
 * @param filename
 * @return set of vector traces
 */
void simple_parser::parse_to_vector(std::ifstream &infile) {
    unique_events->clear();
    vector<string_event> return_vec;
    string line;
    while (std::getline(infile, line)) {
        // if we're at the end of the trace, add the end of trace variable
        // and exit the loop
        if (line == "--") {
            return_vec.push_back(string_event());
            vector_trace_set->insert(return_vec);
            return_vec.clear();
        } else {
            // assumes one event per line, -- terminates
            // constructor w/o terminal event false/true
            return_vec.push_back(string_event(line));
            // if the event is not in the set of events, we add it
            if ((unique_events->find(line) == unique_events->end())) {
                unique_events->insert(line);
            }

        }

    }

    has_been_parsed_vec = true;
}

/**
 * Parses the given file into maps, fills the event set
 * @param filename
 * @return set of map traces
 */
void simple_parser::parse_to_map(std::ifstream &infile) {
    unique_events->clear();
    map<string_event, std::vector<long>> return_map;
    string line;
    long pos_count = 0;
    while (std::getline(infile, line)) {
        // if we're at the end of the trace, add the end of trace variable
        // and exit the loop
        if (line == "--") {
            std::vector<long> pos_vec;
            pos_vec.push_back(pos_count);
            return_map.emplace(string_event(), pos_vec);
            map_trace_set->insert(return_map);
            return_map.clear();
            pos_count = 0;
        } else {
            if ((return_map.find(string_event(line)) == return_map.end())) {
                unique_events->insert(line);
                std::vector<long> pos_vec;
                pos_vec.push_back(pos_count);
                return_map.emplace(string_event(line), pos_vec);
                pos_count++;
            } else {
                return_map.at(string_event(line)).push_back(pos_count);
                pos_count++;
            }
        }
    }
    has_been_parsed_map = true;
}

/**
 * Parses the given file into a vector, fills the event set
 * @param filename
 * @return set of vector traces
 */
void simple_parser::parse_to_pretrees(std::ifstream &infile) {
    unique_events->clear();
    shared_ptr<prefix_tree_node> parent = NULL;
    //std::cout << pre_tree_traces->get_num_traces() << "\n";
    string line;
    int trace_id = 0;
    while (std::getline(infile, line)) {
        // if parent is NULL, we just started a new tree.
        if (parent == NULL) {
            trace_id++;
            shared_ptr<prefix_tree_node> trace_start;
            trace_start = pre_tree_traces->get_trace_start(line);

            // todo: what if two -- follow each other. Let's assume they don't
            if (trace_start == NULL) {
                set<int> trace_ids;
                trace_ids.insert(trace_id);
                shared_ptr<prefix_tree_node> to_insert = std::make_shared<prefix_tree_node>(line,
                        trace_ids);
                pre_tree_traces->add_trace(trace_ids, to_insert);
                parent = pre_tree_traces->get_trace_start(line);
                //std::cout << "Added " << line << " in trace " << trace_id << " as beginning of trace. \n";
            } else {
                //std::cout << "Added trace id " << trace_id << " to start event " << line << ".\n";
                pre_tree_traces->add_id_to_trace(trace_id,trace_start);
                parent = trace_start;
            }
        }
        // if we're at the end of the trace, add the end of trace variable
        // and exit the loop
        else if (line == "--") {
            shared_ptr<prefix_tree_node> twin = parent->get_child(
                    string_event().get_name());
            if (twin != NULL) {
                parent->add_id_to_child(trace_id,twin);
                parent = NULL;

            } else {
                //std::cout << "Ending trace " << trace_id << ". \n";
                set<int> trace_ids;
                trace_ids.insert(trace_id);
                shared_ptr<prefix_tree_node> to_insert = std::make_shared<
                        prefix_tree_node>(trace_ids);
                parent->add_child(to_insert);
                parent = NULL;
            }

        } else {
            // if the event is not in the set of events, we add it
            if ((unique_events->find(line) == unique_events->end())) {
                unique_events->insert(line);
            }
            // check if this event is already a child of the parent
            shared_ptr<prefix_tree_node> twin = parent->get_child(line);
            if (twin != NULL) {
                //std::cout << "Added trace id " << trace_id << " to event " << line << ".\n";
                parent->add_id_to_child(trace_id,twin);
                parent = twin;
            } else {
                //std::cout << "Added " << line << " in trace " << trace_id << " after "<< parent->get_name() <<". \n";
                set<int> trace_ids;
                trace_ids.insert(trace_id);
                shared_ptr<prefix_tree_node> to_insert = std::make_shared<
                        prefix_tree_node>(line, trace_ids);
                parent->add_child(to_insert);
                parent = parent->get_child(line);
            }

        }

    }

    // TODO set has been parsed prefix tree
}

/**
 * Returns the set of parsed unique events
 * @return
 */
shared_ptr<set<string>> simple_parser::return_events() {
    if (has_been_parsed_vec || has_been_parsed_map) {
        return unique_events;
    } else {
        std::cerr << "No files have been parsed, returning empty set. \n";
        return unique_events;
    }
}

/**
 * Returns the set of parsed vector traces
 * @return
 */
shared_ptr<set<vector<string_event> >> simple_parser::return_vec_trace() {
    if (has_been_parsed_vec) {
        return vector_trace_set;
    } else {
        std::cerr << "Trace not parsed into vector, returning empty set. \n";
        return vector_trace_set;
    }

}

/**
 * Returns the set of parsed map traces
 * @return
 */
shared_ptr<set<map<string_event, vector<long>>> > simple_parser::return_map_trace() {
    if (has_been_parsed_map) {
        return map_trace_set;
    } else {
        std::cerr << "Trace not parsed into map, returning empty set. \n";
        return map_trace_set;
    }
}

shared_ptr<prefix_tree> simple_parser::return_prefix_trees(){
//TODO:guard
    return pre_tree_traces;
}

}
/* namespace texada */
