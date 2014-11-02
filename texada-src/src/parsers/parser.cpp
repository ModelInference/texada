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
    trace_separator = "--";
    event_separator = "TODO";   // TODO: come up with appropriate event separator
    ignores_nm_lines = false;
    parse_mult_prop = false;
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
 * Setter for trace separator
 * @param regex: regular expression specifying separators between traces
 */
void parser::set_trace_separator(std::string regex) {
    this->trace_separator = regex;
}

/**
 * Setter for event separator
 * @param regex: regular expression specifying separators between events
 */
void parser::set_event_separator(std::string regex) {
    this->event_separator = regex;
}

/**
 * Configures parser to ignore lines not matching provided regular expressions
 */
void parser::ignore_nm_lines() {
    this->ignores_nm_lines = true;
}

/**
 * Parses the given file, fills the event set
 * @param infile filename
 */
void parser::parse(std::ifstream &infile) {
    unique_events->clear();
    event event;
    while (parser::get_event(infile,event)) {
        add_event(event);
        if (!event.is_terminal()) {
            // Iterate through the parsed event and add any new proposition
            // into our set of unique events
            for (auto p : event.get_props()) {
                if ((parser::unique_events->find(p) == parser::unique_events->end())) {
                    parser::unique_events->insert(p);
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
 * Parses the next event from the given logfile
 * @param infile filename
 * @param event reference for event parsed from file
 * @return whether an event was parsed; return false if end of file has been reached.
 */
bool parser::get_event(std::ifstream &infile, event &event) {
    std::string line;
    std::shared_ptr<std::string> prop;
    event.clear();
    do {
        if (std::getline(infile, line)) {
            if (boost::regex_match(line, trace_separator)) {
                // TODO: handle case when a trace termination is also an event termination
                event = texada::event();
                break;
            } else if (boost::regex_match(line, event_separator)) {
                break;
            } else if ((prop = parse_line(line)) != NULL) {
                event.add_prop(*prop);
            } else if (!ignores_nm_lines) {
                std::cerr << "Error: the line '" + line + "' did not match any of the provided regular expressions. \n";
                exit(1);
            }
        } else {
            return false;
        }
    } while (parse_mult_prop || (ignores_nm_lines && prop == NULL));
    return true;
}

/**
 * Maps a log line to an event type based on provided regular expressions
 * @param log_line
 * @return event type of the given log line or NULL if log line does not match provided regular expressions
 */
shared_ptr<std::string> parser::parse_line(std::string log_line) {

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
 * Helper function called from within parser::parse(std::ifstream
 * &infile) to be overriden by implementing classes. Adds event to the
 * current trace beign built.
 * @param event name
 */
void parser::add_event(event event) {}

} /* namespace texada */
