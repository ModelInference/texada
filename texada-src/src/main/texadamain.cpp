/*
 * Texada main: runs Texada from command line options
 */

#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include "opts.h"
#include <ltlvisit/tostring.hh>
#include <ltlparse/public.hh>

#include "propertytypeminer.h"
#include "../parsing/simpleparser.h"

/**
 * Main method, runs Texada mining or timing tests depending on options.
 */
int main(int ac, char* av[]) {
    try {

        boost::program_options::variables_map opts_map = texada::set_options(ac, av);

        if (opts_map.empty()) {
            std::cerr << "Error: no arguments provided. \n";
            std::cout << texada::get_options_description() << "\n";
            return 1;
        }

        // outputs the option information if --help is inputted
        if (opts_map.count("help")) {
            std::cout << texada::get_options_description() << "\n";
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
            boost::program_options::variables_map file_opts_map = texada::set_options(file_string);
            // insert will not override mapping if it already exists,
            // giving priority to command line opts.
            opts_map.insert(file_opts_map.begin(),file_opts_map.end());
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

