/*
 * parser.cpp
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#include "parser.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

namespace texada {

parser::parser() {
    unique_events = std::make_shared<set<string>>();
    boost::regex expression("(?<ETYPE>.*)");
    event_types.push_back(expression);
    separator_regex = "--";
    ignores_nm_lines = false;
}

parser::~parser() {
}

/**
 * Setter for event_types
 * @param event_types: regular expressions specifying the structure of log
 */
void parser::set_event_types(std::vector<std::string> regexes) {
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
void parser::set_separator(std::string regex) {
    this->separator_regex = regex;
}

/**
 * Configures parser to ignore lines not matching provided regular expressions
 */
void parser::ignore_nm_lines() {
    this->ignores_nm_lines = true;
}

/**
 * Parses the given file, fills the event set
 * @param filename
 */
void parser::parse(std::ifstream &infile) {
    unique_events->clear();
    string line;
    while (std::getline(infile, line)) {
        // if we're at the end of the trace, add the end of trace variable
        // and exit the loop
        if (boost::regex_match(line, separator_regex)) {
            end_trace();
        } else {
            shared_ptr<std::string> etype = parser::get_event_type(line);
            if (etype == NULL) {
                if (ignores_nm_lines) {
                    continue;
                } else {
                    std::cerr << "Error: the line '" + line + "' did not match any of the provided regular expressions. \n";
                    exit(1);
                }
            } else {
                add_event(*etype);
                // if the event is not in the set of events, we add it
                if ((parser::unique_events->find(*etype) == parser::unique_events->end())) {
                    parser::unique_events->insert(*etype);
                }
            }
        }
    }
    has_been_parsed = true;
}

/**
 * Returns the set of parsed unique events
 * @return
 */
shared_ptr<set<string>> parser::return_events() {
    if (has_been_parsed) {
        return unique_events;
    } else {
        std::cerr << "No files have been parsed, returning empty set. \n";
        return unique_events;
    }
}

/**
 * Maps a log line to an event type based on provided regular expressions
 * @param log_line
 * @return event type of the given log line or NULL if log line does not match provided regular expressions
 */
shared_ptr<std::string> parser::get_event_type(std::string log_line) {

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

/**
 * Helper function called from within parser::parse(std::ifstream &infile) to
 * be overriden by implementing class.
 * Ends the current trace being built.
 */
void parser::end_trace() {}

/**
 * Helper function called from within parser::parse(std::ifstream &infile) to
 * be overriden by implementing class.
 * Adds event to the current trace beign built.
 * @param event name
 */
void parser::add_event(std::string event) {}

} /* namespace texada */
