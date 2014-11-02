/*
 * opts.cpp
 *
 *  Created on: Aug 22, 2014
 *      Author: clemieux
 */

#include "opts.h"
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>
#include <iostream>

namespace texada{

/**
 * Get texada options description
 * @return
 */
boost::program_options::options_description get_options_description() {
    // setting up the program options
    // desc is the options description, i.e. all the allowed options
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()("help,h", "produce help message")("property-type,f",
            boost::program_options::value<std::string>(),
            "property type to mine")("log-file",
            boost::program_options::value<std::string>(), "log file to mine on")(
            "map-trace,m", "mine on a trace in the form of a map")(
            "linear-trace,l", "mine on a linear trace")("prefix-tree-trace,p",
            "mine on traces in prefix tree form")("pregen-instants",
            "pregenerate property type instantiations. By default, Texada instantiates them on-the-fly. ")(
            "allow-same-bindings",
            "allow different formula variables to be bound to the same events. By default, Texada does not check instantiations of this type.")(
            "config-file,c", boost::program_options::value<std::string>(),
            "specify file containing command line options. Any options entered directly to command line will override file options.")(
            "event,e",
            boost::program_options::value<std::vector<std::string> >(),
            "specify a variable in the formula to be interpreted as a constant event.")
            ("regex,r", boost::program_options::value<std::vector<std::string> >(),
            "regular expression to parse event types from log [default (?<ETYPE>.*)]")
            ("trace_separator", boost::program_options::value<std::string>(),
            "regular expression matching trace separator lines in the log [default: --]")
            ("event_separator", boost::program_options::value<std::string>(),
            "regular expression matching event separator lines in the log. The set of lines before an event separator are treated as the collection of propositions holding at that event. By default, each line is treated as its own event.")
            ("ignore_nm_lines,i", "ignore non-matching lines [default: false]");
    return desc;

}

/**
 * Get positional options.
 * @return
 */
boost::program_options::positional_options_description get_pos_opts_desc(){
    boost::program_options::positional_options_description pos_desc;
    pos_desc.add("log-file", 1);
    return pos_desc;

}


/**
 * Set the options from string if use_string is true, else from command-line options
 * @param use_string if true, use input string
 * @param input_string
 * @param ac
 * @param av
 * @return
 */
boost::program_options::variables_map set_options(bool use_string,
        std::string input_string, int ac, char* av[]) {

    // setting up the program options
    // desc is the options description, i.e. all the allowed options
    boost::program_options::options_description desc =
            get_options_description();
    boost::program_options::positional_options_description pos_desc = get_pos_opts_desc();


    //parsing the options passed to command line
    boost::program_options::variables_map opts_map;
    if (use_string) {
        std::vector<std::string> args = string_to_args(input_string);
        // Parse the file and store the options
        boost::program_options::store(
                boost::program_options::command_line_parser(args).options(desc).positional(
                        pos_desc).run(), opts_map);

    } else {
        boost::program_options::store(
                boost::program_options::command_line_parser(ac, av).options(
                        desc).positional(pos_desc).run(), opts_map);
    }

    boost::program_options::notify(opts_map);
    return opts_map;

}



/**
 * Set the options from string
 * @param input_string
 * @return
 */
boost::program_options::variables_map set_options(std::string input_string) {
    return set_options(true, input_string, 0, NULL);
}

/**
 * Set the options from string
 * @param input_string
 * @return
 */
boost::program_options::variables_map set_options(int ac, char* av[]) {
    return set_options(false, "", ac, av);
}



/**
 * Parses a block string of commands into the individual arguments
 * for insertion to option maps
 * @param commands
 * @return
 */
std::vector<std::string> string_to_args(std::string commands) {

    // separate arguments by spaces or newlines
    boost::char_separator<char> seperator(" \n\r");
    // split the arguments by spaces or newlines
    boost::tokenizer<boost::char_separator<char> > tok(commands, seperator);

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
                if (it->at((*it).find_first_of("\'\"")) != quote_start_char) {
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
                        quote_parsed_input[quote_start_pos] + " " + (*it);
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

};
