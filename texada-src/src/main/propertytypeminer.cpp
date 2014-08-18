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
#include "../checkers/prefixtreechecker.h"
#include "../instantiation-tools/apsubbingcloner.h"

namespace texada {

/**
 * This method exists to ensure compatibility of tests which used
 * the argument version of mine_property_type. Needs to update to match
 * with texada main.
 * @param input_string
 * @return
 */
boost::program_options::variables_map set_options_from_string(
        string input_string) {
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()("property-type,f",
            boost::program_options::value<std::string>(),
            "property type to mine")("log-file",
            boost::program_options::value<std::string>(), "log file to mine on")(
            "map-trace,m", "mine on a trace in the form of a map")(
            "linear-trace,l", "mine on a linear trace")("prefix-tree-trace,p",
            "mine on traces in prefix tree form")("pregen-instants",
            "pregenerate property type instantiations. ")("allow-same-bindings",
            "allow different formula variables to be bound to the same events.");

    boost::program_options::positional_options_description pos_desc;
    pos_desc.add("log-file", 1);
    boost::program_options::variables_map opts;
    std::vector<std::string> args = texada::string_to_args(input_string);

    boost::program_options::store(
            boost::program_options::command_line_parser(args).options(desc).positional(
                    pos_desc).run(), opts);

    return opts;
}

/**
 * Finds instantiations of the given property type using the linear
 * trace checker. NOTE: this method is only used in testing.
 * @param formula_string the string form of the property type/
 * ltl formula to be mined
 * @param trace_source the input source of the trace
 * @return valid instantiations of the property type on the trace
 */
set<const spot::ltl::formula*> mine_lin_property_type(string formula_string,
        string trace_source) {
    return mine_property_type(
            set_options_from_string(
                    "-f '" + formula_string + "' -l " + trace_source));
}

/**
 * Finds instantiations of the given property type using the map
 * trace checker. NOTE: this method is only used in testing.
 * @param formula_string the string form of the property type/
 * ltl formula to be mined
 * @param trace_source the input source of the trace
 * @return valid instantiations of the property type on the trace
 */
set<const spot::ltl::formula*> mine_map_property_type(string formula_string,
        string trace_source) {
    return mine_property_type(
            set_options_from_string(
                    "-f '" + formula_string + "' -m " + trace_source));
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
set<const spot::ltl::formula*> mine_property_type(
        boost::program_options::variables_map opts) {
    // collect all relevant information from options
    // what trace type to use. TODO: update for pre_tree when added
    bool use_map = opts.count("map-trace");
    bool use_lin = opts.count("linear-trace");
    bool use_pretree = opts.count("prefix-tree-trace");
    // whether to allow repetition in instantiations
    bool allow_reps = opts.count("allow-same-bindings");
    // whether to pregenerate instantiations
    bool pregen_instants = opts.count("pregen-instants");
    // the property type
    string prop_type = opts["property-type"].as<std::string>();
    // trace source
    string trace_source = opts["log-file"].as<std::string>();

    // if any constant events are specified in command line,
    // take these into count
    vector<string> specified_formula_events;
    if (opts.count("event")) {
        specified_formula_events = opts["event"].as<vector<string>>();
    }

    //parse the ltl formula
    spot::ltl::parse_error_list parse_errs;
    const spot::ltl::formula* formula = spot::ltl::parse(prop_type, parse_errs);
    assert(parse_errs.size() == 0);

    // parse file
    std::ifstream infile(trace_source);
    simple_parser parser = simple_parser();
    shared_ptr<set<vector<string_event> >> vector_trace_set;
    shared_ptr<set<map<string_event, vector<long>>> > map_trace_set;
    shared_ptr<prefix_tree> prefix_tree_traces;
    if (use_map) {
        parser.parse_to_map(infile);
        map_trace_set = parser.return_map_trace();
    } else if (use_lin) {
        parser.parse_to_vector(infile);
        vector_trace_set = parser.return_vec_trace();
    } else if (use_pretree) {
        parser.parse_to_pretrees(infile);
        prefix_tree_traces = parser.return_prefix_trees();
    } else {
        std::cerr << "Trace type not specified, nothing gets parsed.";
    }

    shared_ptr<set<string>> event_set = parser.return_events();
    // if we don't want repetition and there are already events
    // in the formula, we can just exclude them from the event set
    // to start with
    if (!allow_reps && (specified_formula_events.size() > 0)) {
        for (int i = 0; i < specified_formula_events.size(); i++) {
            event_set->erase(specified_formula_events.at(i));
        }
    }

    // create the set of formula's variables
    shared_ptr<spot::ltl::atomic_prop_set> variables(
            spot::ltl::atomic_prop_collect(formula));
    // remove events from variables
    if (specified_formula_events.size() > 0) {
        for (spot::ltl::atomic_prop_set::iterator it = variables->begin();
                it != variables->end(); it++) {
            for (int i = 0; i < specified_formula_events.size(); i++) {
                if ((*it)->name() == specified_formula_events.at(i)) {
                    variables->erase(it);
                }
            }
        }
    }

    // create the instantiator
    instants_pool_creator * instantiator;

    if (pregen_instants) {
        instantiator = new pregen_instants_pool(event_set, variables,
                allow_reps, specified_formula_events);
    } else {
        instantiator = new otf_instants_pool(event_set, variables, allow_reps,
                specified_formula_events);
    }

    vector<map<string, string>> valid_instants;
    // check all valid instantiations on each trace
    if (use_lin) {
        valid_instants = valid_instants_on_traces(formula, instantiator,
                vector_trace_set);
    } else if (use_map) {
        valid_instants = valid_instants_on_traces(formula, instantiator,
                map_trace_set);
    } else if (use_pretree) {
        valid_instants = valid_instants_on_traces(formula, instantiator,
                prefix_tree_traces);
    }

    set<const spot::ltl::formula*> return_set;
    int valid_instants_size = valid_instants.size();
    for (int i = 0; i < valid_instants_size; i++) {
        for (map<string, string>::iterator it = valid_instants.at(i).begin();
                it != valid_instants.at(i).end(); it++) {
        }
        const spot::ltl::formula * valid_form = instantiate(formula,
                valid_instants.at(i), specified_formula_events);
        return_set.insert(valid_form);
    }

    delete instantiator;
    formula->destroy();
    return return_set;

}

} /* namespace texada */
