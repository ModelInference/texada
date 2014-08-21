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
shared_ptr<std::string> parser::get_event_type(std::string log_line) {

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
