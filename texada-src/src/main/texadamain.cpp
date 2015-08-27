/*
 * Texada main: runs Texada from command line options
 */

#include <iostream>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <string>
#include <memory>
#include "opts.h"
#include <ltlvisit/tostring.hh>
#include <ltlparse/public.hh>
#include <ltlvisit/apcollect.hh>

#include "propertytypeminer.h"
#include "jsontreeprinter.h"
#include "../instantiation-tools/apsubbingcloner.h"
#include "../parsers/parser.h"
#include "../checkers/statistic.h"

// PRINTING HELPERS

/**
 * Prints a
 * @param instant the instantiation and its statistic
 * @param print_stats whether to print the statistics as well
 */
void print_json(std::ostream & outfile,
        std::pair<std::map<std::string, std::string>, texada::statistic> instant,
        int print_stats) {
    // todo: this needs to take in an outfile
    outfile << "{\"vars\" : {";
    for (std::map<std::string, std::string>::iterator it =
            instant.first.begin(); it != instant.first.end(); it++) {
        if (it != instant.first.begin()) {
            outfile << ", ";
        }
        outfile << "\"" << it->first << "\" : \"" << it->second << "\"";
    }
    outfile << "}";
    if (print_stats) {
        // TODO: might be a better way to do this; do we want to better separate the
        // variable pairs from these ones
        outfile << ", \"stats\" : {";
        outfile << "\"support\" : " << std::to_string(instant.second.support)
                << " ";
        outfile << ", \"support potential\" : "
                << std::to_string(instant.second.support_potential) << " ";
        outfile.precision(5);
        outfile << ", \"confidence\" : " << instant.second.confidence();
        outfile << "}";
    }
    outfile << "}";
}

/**
 * Gets all the locations to to_find in find_in and puts them in posns.
 * @param find_in
 * @param to_find
 * @param posns vector to modify to add positions to
 */
void get_all_locations(std::string find_in, std::string to_find,
        std::vector<int> * posns) {
    std::size_t found = find_in.find(to_find);
    // should terminate since eventually we get to the end of the string
    while (found != std::string::npos) {
        posns->push_back((int) found);
        found = find_in.find(to_find, found + 1); // + 1 assures we won't just loop on the same location
    }

}

/**
 * Removes locations for positions that are covered by the positions in dependencies
 * @param posns
 * @param dependencies
 * @param vec_map
 */
void remove_overlapping_locations(std::vector<int> * posns,
        std::set<std::string> dependencies,
        std::shared_ptr<std::map<std::string, std::vector<int>>>vec_map) {
    for (std::set<std::string>::iterator it = dependencies.begin(); it != dependencies.end(); it++) {
        std::vector<int> dep_posns = vec_map->at(*it);
        std::set<int> invalid_posns;
        int len = it->length();
        for (std::vector<int>::iterator pos = dep_posns.begin(); pos != dep_posns.end(); pos++) {
            for (int i = 0; i< len; i++) {
                invalid_posns.insert((*pos)+ i);
            }
        }
        // here we remove any overlapping positions. Note the predicate for deletion checks
        // if x is in the invalid positions
        auto remove = std::remove_if(posns->begin(), posns->end(),
        [invalid_posns](int x) {return invalid_posns.find(x) != invalid_posns.end();});
        posns->erase(remove);

    }
}

/**
 * Gets the true positions of each atomic proposition in the formula
 * @param formula
 * @param aps
 * @return
 */
std::shared_ptr<std::map<std::string, std::vector<int>>>get_var_pos(std::string formula,
        std::set<std::string> * aps) {
    int max = 0;
    std::shared_ptr<std::map<std::string, std::vector<int>>> ret_map =
    std::shared_ptr<std::map<std::string, std::vector<int>>>(new std::map<std::string, std::vector<int>>());
    std::shared_ptr<std::map<std::string, std::set<std::string>>> dep_map =
    std::shared_ptr<std::map<std::string, std::set<std::string>>>
    (new std::map<std::string, std::set<std::string>>());
    // find all dependencies (i.e. elements each var is a substring of)
    for (std::set<std::string>::iterator it = aps->begin(); it != aps->end(); it++) {
        dep_map->emplace(*it, std::set<std::string>());
        for (std::set<std::string>::iterator v = aps->begin(); v != aps->end(); v++) {
            if ((v->find(*it)!= std::string::npos) & (*v != *it)) {
                dep_map->at(*it).insert(*v);
            }
        }
        max = std::max(max, (int) dep_map->at(*it).size());
    }
    // find all legit positions
    // do this by going through the formula, finding the locations of variables whose
    // 'containing' variables (i.e. the ones they are subsets of) have already been
    // calculated, then removing any locations which overlap with the locations
    // of the 'containing' variables.
    // variables will always have more dependencies than any of their 'containing' vars:
    // let v be a variable and u contain v. then v is contained by u and also by all
    // that contains u: so depend_v contains depend_u and u;
    //  |depend_v| >= |depend_u| + 1 > |depend_u|
    for (int i = 0; i <= max; i++) {
        for (std::map<std::string,std::set<std::string>>::iterator it = dep_map->begin();
                it != dep_map->end(); it++) {
            if (i == it->second.size()) {
                ret_map->emplace(it->first,std::vector<int>());
                get_all_locations(formula, it->first, &(ret_map->at(it->first)));
                if (i != 0) {
                    remove_overlapping_locations(&(ret_map->at(it->first)),
                            it->second, ret_map);
                }
            }
        }
    }
    return ret_map;
}

/**
 * Write "map" in the form {"x" : [2,4], "y" : [6]} to write_on
 * REQUIRES: write_on is already opened
 * @param write_on ofstream to write on
 * @param to_write map to write on ofstream
 */
void print_map_json(std::ostream & write_on,
        std::shared_ptr<std::map<std::string, std::vector<int>>>to_write) {
    write_on << "{";
    for (std::map<std::string,std::vector<int>>::iterator it = to_write->begin();
    it != to_write->end(); it++) {
        if (it != to_write->begin()) {
            write_on << ", ";
        }
        write_on << "\"" << it->first << "\" : [";
        for (std::vector<int>::iterator vec_it = it->second.begin();
        vec_it != it->second.end(); vec_it++) {
            if (vec_it != it->second.begin())
            write_on << ", ";
            write_on << *vec_it;
        }
        write_on << "]";
    }
    write_on << "}";
}

/**
 * Main method, runs Texada mining or timing tests depending on options.
 */
int main(int ac, char* av[]) {
    try {
        boost::program_options::variables_map opts_map = texada::set_options(ac,
                av);

        if (opts_map.empty()) {
            std::cerr << "Error: no arguments provided. \n";
            std::cout << "Usage: texada [opt1] ... [optN] log-file\n";
            std::cout
                    << "Required options: -f, log-file (positional option), one of [-l, -m, -p]\n";
            std::cout << texada::get_options_description() << "\n";
            return 1;
        }

        // outputs the option information if --help is inputted
        if (opts_map.count("help")) {
            std::cout << "Usage: texada [opt1] ... [optN] log-file\n";
            std::cout
                    << "Required options: -f, log-file (positional option), one of [-l, -m, -p]\n";
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
            boost::program_options::variables_map file_opts_map =
                    texada::set_options(file_string);
            // insert will not override mapping if it already exists,
            // giving priority to command line opts.
            opts_map.insert(file_opts_map.begin(), file_opts_map.end());
        }

        //error if no specified trace type
        if (!(opts_map.count("map-trace") || opts_map.count("linear-trace")
                || opts_map.count("prefix-tree-trace"))) {
            std::cerr << "Error: missing a trace representation type. \n";
            return 1;

        }

        // error if specified more than one trace type
        if ((opts_map.count("map-trace") && opts_map.count("linear-trace"))
                || (opts_map.count("map-trace")
                        && opts_map.count("prefix-tree-trace"))
                || (opts_map.count("prefix-tree-trace")
                        && opts_map.count("linear-trace"))) {
            std::cerr
                    << "Error: specify only one trace representation type. \n";
            return 1;
        }

        // error if no property type
        if (!opts_map.count("property-type")) {
            std::cerr << "Error: missing formula. \n";
            return 1;
        }

        // error if no log file
        if (!opts_map.count("log-file")) {
            std::cerr << "Error: missing log file. \n";
            return 1;
        }

        // the set of valid instantiations
        std::vector<
                std::vector<
                        std::pair<std::map<std::string, std::string>,
                                texada::statistic>>> found_instants =
                texada::mine_property_type(opts_map);

        // get prop type
        std::vector<std::string> prop_types = opts_map["property-type"].as<
                std::vector<std::string>>();

        std::vector<const spot::ltl::formula *> formulae;
        std::vector<std::shared_ptr<spot::ltl::atomic_prop_set>> atomic_props_vec;
        std::vector<std::set<std::string>> aps_vec;
        // we'll only get here if there are no parse errors, so no need to check the error list
        spot::ltl::parse_error_list pel = spot::ltl::parse_error_list();
        for (int i = 0; i < prop_types.size(); i++) {
             formulae.push_back(spot::ltl::parse(prop_types[i], pel));
            // get vars of formula
            atomic_props_vec.push_back(std::shared_ptr<spot::ltl::atomic_prop_set>(
                    spot::ltl::atomic_prop_collect(formulae[i])));
            aps_vec.push_back(std::set<std::string>());
            for (spot::ltl::atomic_prop_set::iterator it =
                    atomic_props_vec[i]->begin();
                    it != atomic_props_vec[i]->end(); it++) {
                aps_vec[i].insert((*it)->name());
            }
        }
        std::vector<std::string> specified_formula_events = std::vector<
                std::string>();
        for (int i = 0; i < prop_types.size(); i++) {
            for (std::set<std::string>::iterator it = aps_vec[i].begin();
                    it != aps_vec[i].end(); it++) {
                // if we did not find any instantiations, this is not required
                if (found_instants[i].begin() != found_instants[i].end()) {
                    std::map<std::string, std::string> sample_map =
                            found_instants[i].begin()->first;
                    try {
                        sample_map.at(*it);
                    } catch (std::out_of_range e) {
                        specified_formula_events.push_back(*it);
                    }
                }
            }
        }

        std::ofstream realOutFile;
        if (opts_map.count("out-file"))
            realOutFile.open(opts_map["out-file"].as<std::string>(),
                    std::ios::out);

        std::ostream & outFile = (
                opts_map.count("out-file") ? realOutFile : std::cout);

        if (opts_map.count("output-json")) {
            outFile << "{";
        }
        for (int i = 0; i < prop_types.size(); i++) {
            if (i != 0) {
                outFile << ", ";
            }
            if (opts_map.count("output-json")) {
                std::shared_ptr<std::map<std::string, std::vector<int>>>var_pos_map =
                get_var_pos(prop_types[i], &aps_vec[i]);
                outFile << "{\"prop-type\": {\"str\": \"" << prop_types[i] << "\", \"vars\" : ";
                print_map_json(outFile,var_pos_map);
                outFile << ", \"tree\": ";
                texada::json_tree_printer printer = texada::json_tree_printer(outFile);
                formulae[i]->accept(printer);
                outFile << "}, \"prop-instances\" : [";

            }
            // print out all the valid instantiations
            for (std::vector<
                    std::pair<std::map<std::string, std::string>,
                            texada::statistic>>::iterator it =
                    found_instants[i].begin(); it != found_instants[i].end(); it++) {
                if (opts_map.count("output-json")) {
                    if (it != found_instants[i].begin())
                        outFile << ", ";
                    print_json(outFile, *it, opts_map.count("print-stats"));
                } else {
                    const spot::ltl::formula * valid_form = texada::instantiate(
                            formulae[i], it->first, specified_formula_events);
                    outFile << spot::ltl::to_string(valid_form) << "\n";
                    valid_form->destroy();

                    // if printing is turned on, print the statistics of each valid finding
                    // warning: printing statistics incurs expensive overhead as it disables
                    // all possible short-circuiting within the checker.
                    if (opts_map.count("print-stats")) {
                        outFile
                                << "   support: "
                                        + std::to_string(it->second.support)
                                << "\n";
                        outFile
                                << "   support potential: "
                                        + std::to_string(
                                                it->second.support_potential)
                                << "\n";
                        outFile << "   confidence: ";
                        outFile.precision(5);
                        outFile << it->second.confidence();
                        outFile << "\n";
                    }
                }
            }
            if (opts_map.count("output-json")) {
                outFile << "]}";
            }

        }
        if (opts_map.count("output-json")) {
            outFile << "}\n";
        }

        if (opts_map.count("out-file")) {
            realOutFile.close();
        }
        for (int i = 0; i < formulae.size(); i++) {
            formulae[i]->destroy();
        } //might not be ok
          // exception catching
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

