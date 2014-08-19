/*
 * Texada main: runs Texada from command line options
 */

#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include <boost/program_options.hpp>
#include <ltlvisit/tostring.hh>
#include <ltlparse/public.hh>
#include "propertytypeminer.h"
#include "../parsing/simpleparser.h"

/**
 * Main method, runs Texada mining or timing tests depending on options.
 */
int main(int ac, char* av[]) {
    try {

        // setting up the program options
        // desc is the options description, i.e. all the allowed options
        boost::program_options::options_description desc("Allowed options");
        desc.add_options()("help,h", "produce help message")("property-type,f",
                boost::program_options::value<std::string>(),
                "property type to mine")("log-file",
                boost::program_options::value<std::string>(),
                "log file to mine on")("map-trace,m",
                "mine on a trace in the form of a map")("linear-trace,l",
                "mine on a linear trace")("prefix-tree-trace,p",
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
                "event types in log file. Written 'a(?<ETYPE>x)b' where a,b,x are regular expressions and x indicates a matching line's event type [default (?<ETYPE>.*)]")
                ("separator_regex,s", boost::program_options::value<std::string>(), "regular expression matching lines which separate the log into different traces [default --]")
                ("ignore_nm_lines,i", "ignore lines that do not match any of the passed regular expressions");

        boost::program_options::positional_options_description pos_desc;
        pos_desc.add("log-file", 1);

        //parsing the options passed to command line
        boost::program_options::variables_map opts_map;
        boost::program_options::store(
                boost::program_options::command_line_parser(ac, av).options(
                        desc).positional(pos_desc).run(), opts_map);
        boost::program_options::notify(opts_map);

        if (opts_map.empty()) {
            std::cerr << "Error: no arguments provided. \n";
            std::cout << desc << "\n";
            return 1;
        }

        // outputs the option information if --help is inputted
        if (opts_map.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        // if options are specified in a response file, use it instead.
        // code mostly borrowed from boost example/response_file.cpp
        if (opts_map.count("config-file")) {
            std::string input_string =
                    opts_map["config-file"].as<std::string>();
            std::ifstream infile(input_string);
            if (!infile) {
                std::cerr << "Error: could not open the config file.\n";
                return 1;
            }
            // Read the whole file into a string
            std::stringstream file_string_stream;
            file_string_stream << infile.rdbuf();
            std::string file_string = file_string_stream.str();
            std::vector<std::string> args = texada::string_to_args(file_string);
            // Parse the file and store the options
            boost::program_options::store(
                    boost::program_options::command_line_parser(args).options(
                            desc).positional(pos_desc).run(), opts_map);

        }

        //error if no specified trace type
        if (!(opts_map.count("map-trace") || opts_map.count("linear-trace")
                || opts_map.count("prefix-tree-trace"))) {
            std::cerr << "Error: specify a trace type. \n";
            return 1;

        }

        // error if specified more than one trace type
        if ((opts_map.count("map-trace") && opts_map.count("linear-trace"))
                || (opts_map.count("map-trace")
                        && opts_map.count("prefix-tree-trace"))
                || (opts_map.count("prefix-tree-trace")
                        && opts_map.count("linear-trace"))) {
            std::cerr << "Error: specify only one trace type. \n";
            return 1;
        }

        // error if no property type
        if (!opts_map.count("property-type")) {
            std::cerr << "Error: no inputted property type. \n";
            return 1;
        }

        // error if no log file
        if (!opts_map.count("log-file")) {
            std::cerr << "Error: did not provide log file. \n";
            return 1;
        }

        // the set of valid instantiations
        std::set<const spot::ltl::formula*> found_instants =
                texada::mine_property_type(opts_map);
        // print out all the valid instantiations as return
        for (std::set<const spot::ltl::formula*>::iterator it =
                found_instants.begin(); it != found_instants.end(); it++) {
            std::cout << spot::ltl::to_string(*it) << "\n";
        }

        // exception catching
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

