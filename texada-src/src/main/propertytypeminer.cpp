/*
 * propertytypeminer.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: clemieux
 */

#include "propertytypeminer.h"

#include <fstream>
#include <assert.h>

#include <ltlparse/public.hh>
#include <ltlvisit/apcollect.hh>
#include <ltlvisit/tostring.hh>

#include <boost/program_options.hpp>

#include "../parsing/simpleparser.h"
#include "../instantiation-tools/pregeninstantspool.h"
#include "../instantiation-tools/otfinstantspool.h"
#include "../instantiation-tools/instantspoolcreator.h"
#include "../checkers/maptracechecker.h"
#include "../checkers/lineartracechecker.h"
#include "../instantiation-tools/apsubbingcloner.h"

namespace texada {

boost::program_options::variables_map set_options_from_string(string input_string){
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()("property_type,f",
            boost::program_options::value<std::string>(),
            "property type to mine")("log_file",
            boost::program_options::value<std::string>(),
            "log file to mine on")(
            "map_trace,m", "mine on a trace in the form of a map")(
            "linear_trace,l", "mine on a linear trace")("pregen_instants,p",
            "pregenerate property type instantiations. ")(
            "allow_same_bindings",
            "allow different formula variables to be bound to the same events.");

    boost::program_options::positional_options_description pos_desc;
    pos_desc.add("log_file", 1);
    boost::program_options::variables_map opts;
    std::vector<std::string> args =texada::string_to_args(input_string);

    boost::program_options::store(
            boost::program_options::command_line_parser(args).options(
                    desc).positional(pos_desc).run(), opts);

    return opts;
}

/**
 * Finds instantiations of the given property type using the linear
 * trace checker
 * @param formula_string the string form of the property type/
 * ltl formula to be mined
 * @param trace_source the input source of the trace
 * @return valid instantiations of the property type on the trace
 */
set<const spot::ltl::formula*> mine_lin_property_type(string formula_string,
        string trace_source) {
    return mine_property_type(set_options_from_string("-f " + formula_string + " -l " + trace_source));
}

/**
 * Finds instantiations of the given property type using the map
 * trace checker
 * @param formula_string the string form of the property type/
 * ltl formula to be mined
 * @param trace_source the input source of the trace
 * @return valid instantiations of the property type on the trace
 */
set<const spot::ltl::formula*> mine_map_property_type(string formula_string,
        string trace_source) {
    return mine_property_type(set_options_from_string("-f " + formula_string + " -m " + trace_source));
}

/**
 * Finds valid instantiations of the given property type on the given
 * traces, using map or linear checker according to use_map.
 * @param formula_string the string form of the property type/
 * ltl formula to be mined
 * @param trace_source the input source of the trace
 * @param use_map use map miner if true, linear miner otherwise
 * @return valid instantiations of the inputted formula on inputted trace set
 */
set<const spot::ltl::formula*> mine_property_type(boost::program_options::variables_map opts) {
    bool use_map = opts.count("map_trace");
    bool allow_reps = opts.count("allow_same_bindings");
    bool pregen_instants = opts.count("pregen_instants");
    string prop_type = opts["property_type"].as<std::string>();
    string trace_source = opts["log_file"].as<std::string>();

    //parse the ltl formula
    spot::ltl::parse_error_list parse_errs;
    const spot::ltl::formula* formula = spot::ltl::parse(prop_type,
            parse_errs);
    assert(parse_errs.size() == 0);

    // parse file
    std::ifstream infile(trace_source);
    simple_parser * parser = new simple_parser();
    shared_ptr<set<vector<string_event> >> vector_trace_set;
    shared_ptr<set<map<string_event, vector<long>>> > map_trace_set;
    if (use_map) {
        parser->parse_to_map(infile);
        map_trace_set = parser->return_map_trace();

    } else {
        parser->parse_to_vector(infile);
        vector_trace_set = parser->return_vec_trace();
    }

    shared_ptr<set<string>> event_set = parser->return_events();
    //done parsing file, clean up parser
    delete parser;
    parser = NULL;

    // create the set of formula's variables
    shared_ptr<spot::ltl::atomic_prop_set> variables(spot::ltl::atomic_prop_collect(
            formula));
    // create the instantiator
    instants_pool_creator * instantiator;

    if (pregen_instants) {
        instantiator = new pregen_instants_pool(event_set, variables,
                allow_reps);
    } else {
        instantiator = new otf_instants_pool(event_set, variables, allow_reps);
    }


    vector<map<string, string>> valid_instants;
    // check all valid instantiations on each trace
    if (!use_map) {
        valid_instants = valid_instants_on_traces(formula, instantiator,
                vector_trace_set);
    } else {
        valid_instants = valid_instants_on_traces(formula, instantiator,
                map_trace_set);
    }



    set<const spot::ltl::formula*> return_set;

    int valid_instants_size = valid_instants.size();
    for (int i = 0; i < valid_instants_size; i++) {
        const spot::ltl::formula * valid_form = instantiate(formula,
                valid_instants.at(i));
        return_set.insert(valid_form);
    }

    delete instantiator;
    formula->destroy();
    return return_set;

}

} /* namespace texada */
