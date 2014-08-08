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
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

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
 * Parses the given file into prefix trees, fills the event set
 * @param filename
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

/**
 * Returns the parsed prefix trees:
 * TODO: guard
 * @return
 */
shared_ptr<prefix_tree> simple_parser::return_prefix_trees(){
    return pre_tree_traces;
}


/**
 * Parses a block string of commands into the individual arguments
 * for insertion to option maps
 * @param commands
 * @return
 */
std::vector<std::string> string_to_args(std::string commands){

    // separate arguments by spaces or newlines
    boost::char_separator<char> seperator(" \n\r");
    // split the arguments by spaces or newlines
    boost::tokenizer<boost::char_separator<char> > tok(commands,
            seperator);

    //check for any quotes: want to not split things inside quotes
    // to store the args after having fixed quote issues
    std::vector<std::string> quote_parsed_input;
    // to know our state inside quotes
    bool inside_quotes = false;
    bool ended_quote = true;
    // where in the quote_parsed_input is the current quote
    int quote_start_pos;
    // is this a ' or "
    char quote_start_char;
    // go through the first tokenized parsing
    for (boost::tokenizer<boost::char_separator<char> >::iterator it =
            tok.begin(); it != tok.end(); it++) {
        if (inside_quotes == true) {
            // if inside quotes and we found an end quote:
            if ((*it).find_first_of("\'\"") != std::string::npos) {
                // if it's not the correct endquote, error
                if (it->at((*it).find_first_of("\'\""))
                        != quote_start_char) {
                    std::cerr << "Error: mismatched quotes. \n";
                    //TODO: throw excpetion
                }
                // if it's the correct end quote, we've ended our quote
                ended_quote = true;
                inside_quotes = false;
                std::string element = std::string(*it);
                // add the end part of the quote to the quoted element
                // in the args vector
                quote_parsed_input[quote_start_pos] =
                        quote_parsed_input[quote_start_pos] + " "
                                + element.substr(0,
                                        (*it).find_first_of("\'\""));
            } else {
                // if we're inside quotes, just stick this part
                // onto the inside of quotes
                quote_parsed_input[quote_start_pos] =
                        quote_parsed_input[quote_start_pos] + " "
                                + (*it);
            }

        } else if ((*it).find_first_of("\'\"") == 0) {
            //TODO: missing some cases, i.e. "ah"a
            // if we just found an opening quote, but there's an end
            // quote in this token, just push in the quoteless object
            if ((*it).find_last_of("\'\"") == (*it).length() - 1) {
                std::string first_element = std::string(*it);
                quote_parsed_input.push_back(
                        first_element.substr(1, (*it).length() - 2));

            } else {
                // if we just found an opening quote and it's not a one-block
                // quote, we're inside quotes
                inside_quotes = true;
                ended_quote = false;
                quote_start_char = it->at((*it).find_first_of("\'\""));
                quote_start_pos = quote_parsed_input.size();
                std::string first_element = std::string(*it);
                quote_parsed_input.push_back(first_element.substr(1));
            }
        }
        // if we're not inside a quote and we didn't find a quote,
        // just push it normally
        else {
            quote_parsed_input.push_back(*it);
        }
    }
    //TODO: can I just use inside_quotes instead
    if (!ended_quote) {
        std::cerr << "Error: missing \' or \". \n";
        //TODO: throw exception
    }

    std::vector<std::string> args;
    // copy the options into args
    std::copy(quote_parsed_input.begin(), quote_parsed_input.end(),
            back_inserter(args));
    return args;

}

} /* namespace texada */
