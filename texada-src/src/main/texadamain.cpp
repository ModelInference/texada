/*
 * Texada main: runs Texada from command line options
 */

#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>
#include <ltlvisit/tostring.hh>
#include <ltlparse/public.hh>
#include "propertytypeminer.h"
#include "../parsing/simpleparser.h"
#include "../checkers/truncatingchecker.h"

/**
 * Runs Texada, timing and outputting to std::cout the mining.
 * @param form property type to mine
 * @param source input file to mine
 * @param use_map use map miner if true
 */
void set_up_timed_mining(std::string form, std::string source, bool use_map) {
    clock_t begin, end;
    double time_spent;
    begin = clock();
    texada::mine_property_type(form, source, use_map);
    end = clock();
    time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    std::cout << time_spent << "\n";

}

void set_up_timed_mining_temp_truncator(std::string form, std::string source) {
    clock_t begin, end;
    double time_spent;
    begin = clock();
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* prop_type = spot::ltl::parse(form, pel);
    texada::simple_parser * parser = new texada::simple_parser();
    std::ifstream infile(source);
    parser->parse_to_vector(infile);
    texada::truncating_checker checker = texada::truncating_checker(prop_type,
            parser->return_events());
    checker.return_valid_instants(prop_type, parser->return_vec_trace());
    delete parser;
    prop_type->destroy();
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
void mine_on_increasing_events(std::string form, bool use_map) {
    std::string trace_base = std::string(getenv("TEXADA_HOME"))
            + "/traces/vary-invs-fixed2/log-25000_invs-";

    // we'll mine the property type represented by form on the traces with
    // the number of unique events in the array below
    int unique_event_nums[] = { 5, 6, 7, 8, 9, 13, 16, 19, 22, 25, 28, 31, 34,
            37, 40, 43, 46, 50, 60, 70, 80 };
    std::cout << form;
    // output map trace cout if use_map. Just a shortedned if/else.
    use_map ?
            std::cout << ", map trace: \n" : std::cout << ", linear trace: \n";

    // go through the traces with unique event numbers given from the array
    // and output time
    for (int i = 0; i < (sizeof(unique_event_nums) / sizeof(int)); i++) {
        std::cout << unique_event_nums[i] << " ";
        set_up_timed_mining(form,
                trace_base + std::to_string(unique_event_nums[i] - 1) + ".txt",
                use_map);

    }
}

/**
 * Main method, runs Texada mining or timing tests depending on options.
 */
int main(int ac, char* av[]) {
    try {
        // input property type
        std::string prop_type;
        // log file source
        std::string input_source;
        //
        bool use_map = false;

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
                "truncating_checker,t", "mine w/ trunc checker")
                ("allow_same_bindings", "allow different formula variables to be bound to the same events. By default, Texada does not check instantiations of this type.")(
                "config_file,c", boost::program_options::value<std::string>(),
                "specify file containing command line options. Any options entered directly to command line will override file options.");

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
            // Split the file content
            boost::char_separator<char> seperator(" \n\r");
            std::string file_string = file_string_stream.str();
            boost::tokenizer<boost::char_separator<char> > tok(file_string,
                    seperator);
            std::vector<std::string> quote_parsed_input;
            //check for any quotes
            bool inside_quotes = false;
            bool ended_quote = true;
            int quote_start_pos;
            for (boost::tokenizer<boost::char_separator<char> >::iterator it =
                    tok.begin(); it != tok.end(); it++) {
                if (inside_quotes == true) {
                    if ((*it).find_first_of("\'\"") != std::string::npos) {
                        ended_quote = true;
                        inside_quotes = false;
                        std::string element = std::string(*it);
                        quote_parsed_input[quote_start_pos] =
                                quote_parsed_input[quote_start_pos] + " "
                                        + element.substr(0,
                                                (*it).find_first_of("\'\""));
                    } else {
                        quote_parsed_input[quote_start_pos] =
                                quote_parsed_input[quote_start_pos] + " "
                                        + (*it);
                    }

                } else if ((*it).find_first_of("\'\"") == 0) {
                    if ((*it).find_last_of("\'\"") == (*it).length() - 1) {
                        std::string first_element = std::string(*it);
                        quote_parsed_input.push_back(
                                first_element.substr(1, (*it).length() - 2));

                    } else {
                        inside_quotes = true;
                        ended_quote = false;
                        quote_start_pos = quote_parsed_input.size();
                        std::string first_element = std::string(*it);
                        quote_parsed_input.push_back(first_element.substr(1));
                    }
                } else {
                    quote_parsed_input.push_back(*it);
                }
            }
            if (!ended_quote) {
                std::cerr << "Error: missing \' or \". \n";
                return 1;
            }

            std::vector<std::string> args;
            // copy the options into args
            std::copy(quote_parsed_input.begin(), quote_parsed_input.end(),
                    back_inserter(args));
            // Parse the file and store the options
            boost::program_options::store(
                    boost::program_options::command_line_parser(args).options(
                            desc).positional(pos_desc).run(), opts_map);
            //   std::cout << opts_map["property_type"].as<std::string>() << "\n";

        }

        if (!opts_map.count("map_trace")&& !opts_map.count("linear_trace")){
            std::cerr << "Error: did not specify map or linear trace. \n";
            return 1;
        }

        // if the user wanted to use map, we use map.
        if (opts_map.count("map_trace"))
            use_map = true;

        if (opts_map.count("linear_trace"))
            use_map = false;


        // places the inputed property type into the prop_type;
        // returns with error if there is none
        if (opts_map.count("property_type")) {
            prop_type = opts_map["property_type"].as<std::string>();
        } else {
            std::cerr << "Error: no inputted property type. \n";
            return 1;
        }

        // runs the timing on increasing unique events if specified
        if (opts_map.count("run_on_increasing_events")) {
            mine_on_increasing_events(prop_type, use_map);
            return 0;
        }

        // sets the log file source to input
        if (opts_map.count("log_file")) {
            input_source = opts_map["log_file"].as<std::string>();
        } else {
            std::cerr << "Error: did not provide log file. \n";
            return 1;
        }

        //TODO: fix
        if (opts_map.count("truncating_checker")) {
            set_up_timed_mining_temp_truncator(prop_type, input_source);
            std::cout << "hello \n";
            return 0;
        }
        // the set of valid instantiations
        std::set<const spot::ltl::formula*> found_instants;

        // put valid instantiations in the correct variable
        if (use_map) {
            found_instants = texada::mine_map_property_type(prop_type,
                    input_source);
        } else {

            found_instants = texada::mine_lin_property_type(prop_type,
                    input_source);
        }

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

