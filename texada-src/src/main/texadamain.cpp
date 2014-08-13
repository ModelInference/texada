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
 * Runs Texada, timing and outputting to std::cout the mining.
 * @param form property type to mine
 * @param source input file to mine
 * @param use_map use map miner if true
 */
void set_up_timed_mining(boost::program_options::variables_map opts_map) {
    clock_t begin, end;
    double time_spent;
    begin = clock();
    texada::mine_property_type(opts_map);
    end = clock();
    time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    std::cout << time_spent << "\n";

}

/**
 * Mines form on on traces with an increasing number of increasing
 * events, outputting the time it takes for each.
 * @param form property type to mine
 * @param use_map
 */
void mine_on_increasing_events(boost::program_options::variables_map opts_map) {
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        return;
    }
    std::string trace_base = std::string(getenv("TEXADA_HOME"))
            + "/traces/vary-invs-fixed2/log-25000_invs-";

    // we'll mine the property type represented by form on the traces with
    // the number of unique events in the array below
    int unique_event_nums[] = { 5, 6, 7, 8, 9, 13, 16, 19, 22, 25, 28, 31, 34,
            37, 40, 43, 46, 50, 60, 70, 80 };
    std::cout << opts_map["property_type"].as<std::string>();
    // output map trace cout if use_map. Just a shortedned if/else.
    opts_map.count("map_trace") ?
            std::cout << ", map trace: \n" : std::cout << ", linear trace: \n";

    // go through the traces with unique event numbers given from the array
    // and output time
    for (int i = 0; i < (sizeof(unique_event_nums) / sizeof(int)); i++) {
        std::cout << unique_event_nums[i] << " ";
        //opts_map.emplace("log_file",trace_base + std::to_string(unique_event_nums[i] - 1) + ".txt");
        set_up_timed_mining(opts_map);

    }
}

/**
 * Main method, runs Texada mining or timing tests depending on options.
 */
int main(int ac, char* av[]) {
    try {

        // setting up the program options
        // desc is the options description, i.e. all the allowed options
        boost::program_options::options_description desc("Allowed options");
        desc.add_options()("help,h", "produce help message")("property_type,f",
                boost::program_options::value<std::string>(),
                "property type to mine")("log_file",
                boost::program_options::value<std::string>(),
                "log file to mine on")("run_on_increasing_events",
                "run the prop type through traces with increasing number of unique events")(
                "map_trace,m", "mine on a trace in the form of a map")(
                "linear_trace,l", "mine on a linear trace")("pregen_instants,p",
                "pregenerate property type instantiations. By default, Texada instantiates them on-the-fly. ")(
                "allow_same_bindings",
                "allow different formula variables to be bound to the same events. By default, Texada does not check instantiations of this type.")(
                "config_file,c", boost::program_options::value<std::string>(),
                "specify file containing command line options. Any options entered directly to command line will override file options.")(
                "event,e",
                boost::program_options::value<std::vector<std::string> >(),
                "specify a variable in the formula to be interpreted as a constant event.");

        boost::program_options::positional_options_description pos_desc;
        pos_desc.add("log_file", 1);

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
        if (opts_map.count("config_file")) {
            // std::cout << opts_map["property_type"].as<std::string>() << "\n";
            std::string input_string =
                    opts_map["config_file"].as<std::string>();
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

        // error if specified more than one or no trace type
        if (!(opts_map.count("map_trace") xor opts_map.count("linear_trace"))) {
            std::cerr << "Error: specify one trace type. \n";
            return 1;
        }

        // error if no property type
        if (!opts_map.count("property_type")) {
            std::cerr << "Error: no inputted property type. \n";
            return 1;
        }

        // runs the timing on increasing unique events if specified
        if (opts_map.count("run_on_increasing_events")) {
            mine_on_increasing_events(opts_map);
            return 0;
        }

        // error if no log file
        if (!opts_map.count("log_file")) {
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

