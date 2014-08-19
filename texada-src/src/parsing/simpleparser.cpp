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
    boost::regex expression("(?<ETYPE>.*)");
    event_types.push_back(expression);
    separator_regex = "--";
    ignores_nm_lines = false;
}

simple_parser::~simple_parser() {
}

/**
 * Setter for event_types
 * @param event_types: regular expressions specifying the structure of log
 */
void simple_parser::set_event_types(std::vector<std::string> regexes) {
    this->event_types.clear();
    for (auto & element : regexes) {
        boost::regex expression(element);
        this->event_types.push_back(expression);
    }
}

/**
 * Setter for separator_regex
 * @param regex: regular expression specifying terminating event
 */
void simple_parser::set_separator(std::string regex) {
    this->separator_regex = regex;
}

/**
 * Configures parser to ignore lines not matching provided regular expressions
 */
void simple_parser::ignore_nm_lines() {
    this->ignores_nm_lines = true;
}


// TODO ib: the parse_to_* methods below have the same structure --
// retrieve a line from a file, match a separator reg expression
// against the line, modify some data structures, otherwise check if
// it matches a reg exp for event parsing, and modify/create more
// structures. Refactor all of these methods to use a common single
// method that provides this structure. Create specialized instances
// that parse vectors/maps/whatever using inheritance (simplest), or
// function pointers, or other means of indirection. In a sense, these
// share structure and we have to refactor this structure out into a
// reusable piece.

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
        if (boost::regex_match(line, separator_regex)) {
            return_vec.push_back(string_event());
            vector_trace_set->insert(return_vec);
            return_vec.clear();
        } else {

          // TODO ib: indentation difference in else branch -- maintain indent consistency.

          // TODO ib: there is a 'tab' character in indent of next line. Do _not_ use tabs anywhere in the code.

            // get event type of line
            shared_ptr<std::string> etype = simple_parser::get_event_type(line);
            // if etype is NULL, line does not match any of the provided regular expressions
            // ignore or exit program based on value of ignores_nm_lines
            if (etype == NULL) {
                if (ignores_nm_lines) {
                    continue;
                } else {
                    // TODO: consider returning line number
                    std::cerr << "Error: the line '" + line + "' did not match any of the provided regular expressions. \n";
                    exit(1);
                }
            }
            // assumes one event per line, line matching separator_regex terminates
            // constructor w/o terminal event false/true
            return_vec.push_back(string_event(*etype));
            // if the event is not in the set of events, we add it
            if ((unique_events->find(*etype) == unique_events->end())) {
                unique_events->insert(*etype);
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
        if (boost::regex_match(line, separator_regex)) {
            std::vector<long> pos_vec;
            pos_vec.push_back(pos_count);
            return_map.emplace(string_event(), pos_vec);
            map_trace_set->insert(return_map);
            return_map.clear();
            pos_count = 0;
        } else {
            // TODO ib: the next 14 lines or seem to be copy-pasted
            // from above. Avoid copy-pasted code like the
            // plague. Refactor identical code into reusable methods.

            // get event type of line
            shared_ptr<std::string> etype = simple_parser::get_event_type(line);
            // if etype is NULL, line does not match any of the provided regular expressions
            // ignore or exit program based on value of ignores_nm_lines
            //std::cout << *etype << "\n";
            if (etype == NULL) {
                if (ignores_nm_lines) {
                    continue;
                } else {
                    // TODO: consider returning line number
                    std::cerr << "Error: the line '" + line + "' did not match any of the provided regular expressions. \n";
                    exit(1);
                }
            }
            if ((return_map.find(string_event(*etype)) == return_map.end())) {
                unique_events->insert(*etype);
                std::vector<long> pos_vec;
                pos_vec.push_back(pos_count);
                return_map.emplace(string_event(*etype), pos_vec);
                pos_count++;
            } else {
                return_map.at(string_event(*etype)).push_back(pos_count);
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
            if ((unique_events->find(line) == unique_events->end())) {
                unique_events->insert(line);
            }
            trace_id++;
            shared_ptr<prefix_tree_node> trace_start;
            trace_start = pre_tree_traces->get_trace_start(line);
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
        else if (boost::regex_match(line, separator_regex)) {
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
            // TODO ib: more copied code....

            // get event type of line
            shared_ptr<std::string> etype = simple_parser::get_event_type(line);
            // if etype is NULL, line does not match any of the provided regular expressions
            // ignore or exit program based on value of ignores_nm_lines
            if (etype == NULL) {
                if (ignores_nm_lines) {
                    continue;
                } else {
                    // TODO: consider returning line number
                    std::cerr << "Error: the line '" + line + "' did not match any of the provided regular expressions. \n";
                    exit(1);
                }
            }
            // if the event is not in the set of events, we add it
            if ((unique_events->find(*etype) == unique_events->end())) {
                unique_events->insert(*etype);
            }
            // check if this event is already a child of the parent
            shared_ptr<prefix_tree_node> twin = parent->get_child(*etype);
            if (twin != NULL) {
                //std::cout << "Added trace id " << trace_id << " to event " << line << ".\n";
                parent->add_id_to_child(trace_id,twin);
                parent = twin;
            } else {
                //std::cout << "Added " << line << " in trace " << trace_id << " after "<< parent->get_name() <<". \n";
                set<int> trace_ids;
                trace_ids.insert(trace_id);
                shared_ptr<prefix_tree_node> to_insert = std::make_shared<
                        prefix_tree_node>(*etype, trace_ids);
                parent->add_child(to_insert);
                parent = parent->get_child(*etype);
            }

        }

    }

    has_been_parsed_pretree = true;
}

/**
 * Returns the set of parsed unique events
 * @return
 */
shared_ptr<set<string>> simple_parser::return_events() {
    if (has_been_parsed_vec || has_been_parsed_map || has_been_parsed_pretree) {
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
    if (has_been_parsed_pretree) {
        return pre_tree_traces;
    } else {
        std::cerr << "Trace not parsed into pre trees, returning empty prefix trees. \n";
        return pre_tree_traces;
    }


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

/**
 * Maps a log line to an event type based on provided regular expressions
 * @param log_line
 * @return event type of the given log line or NULL if log line does not match provided regular expressions
 */
shared_ptr<std::string> simple_parser::get_event_type(std::string log_line) {

  // TODO ib: more tabs and inconsistent indentation level below; fix this across all files.

    boost::smatch results;
    for (auto & e : this->event_types) {
        if (boost::regex_match(log_line, results, e)) {
            shared_ptr<std::string> etype (new string(results["ETYPE"]));
            return etype;
        }
    }
    // log_line did not match any of the provided regular expressions
    return NULL;
}

} /* namespace texada */
