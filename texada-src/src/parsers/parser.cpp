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
    unique_props = std::make_shared<set<string>>();
    boost::regex expression("(?<ETYPE>.*)");
    prop_types.push_back(expression);
    trace_separator = "--";
    event_separator = "..";
    ignores_nm_lines = false;
    parses_mult_prop = false;
}

parser::~parser() {
}

/**
 * Setter for prop_types
 * @param prop_types: regular expressions specifying the structure of log
 */
void parser::set_prop_types(std::vector<std::string> regexes) {
    this->prop_types.clear();
    for (auto & element : regexes) {
        boost::regex expression(element);
        this->prop_types.push_back(expression);
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
 * Configures parser to parse multi-propositional log
 */
void parser::parse_mult_prop() {
    this->parses_mult_prop = true;
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
    unique_props->clear();
    event event;
    while (parser::get_event(infile,event)) {
        add_event(event);
    }
    has_been_parsed = true;
}

/**
 * Returns the set of parsed unique events
 * @return
 */
shared_ptr<set<string>> parser::return_props() {
    if (has_been_parsed) {
        return unique_props;
    } else {
        std::cerr << "No files have been parsed, returning empty set. \n";
        return unique_props;
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
                // if trace termination is also being used as an event termination
                // add the current event followed by a terminal event.
                if (!event.is_empty()) {
                    add_event(event);
                }
                event = texada::event();
                break;
            } else if (parses_mult_prop && boost::regex_match(line, event_separator)) {
                break;
            } else if ((prop = parse_line(line)) != NULL) {
                event.add_prop(*prop);
            } else if (!ignores_nm_lines) {
                std::cerr << "Error: the line '" + line + "' did not match any of the provided regular expressions. \n";
                exit(1);
            }
        } else {
            // TODO: handle case when an EOF is also an event termination
            return false;
        }
    } while (parses_mult_prop || (ignores_nm_lines && prop == NULL));
    return true;
}

/**
 * Maps a log line to an event type based on provided regular expressions
 * @param log_line
 * @return event type of the given log line or NULL if log line does not match provided regular expressions
 */
shared_ptr<std::string> parser::parse_line(std::string log_line) {
    boost::smatch results;
    for (auto & e : this->prop_types) {
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
 * &infile) to be extended by implementing classes. First, adds all
 * unique propositions in given event to the list of unique propositions,
 * then adds event to the current trace being built.
 * @param event name
 */
void parser::add_event(event event) {
    if (!event.is_terminal()) {
        // Iterate through the parsed event and add any new proposition
        // into our set of unique events.
        for (auto p : event.get_props()) {
            if ((parser::unique_props->find(p) == parser::unique_props->end())) {
                parser::unique_props->insert(p);
            }
        }
    }
}

} /* namespace texada */
