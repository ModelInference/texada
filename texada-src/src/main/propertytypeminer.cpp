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

#include "../parsing/simpleparser.h"
#include "../instantiation-tools/pregeninstantspool.h"
#include "../instantiation-tools/otfinstantspool.h"
#include "../instantiation-tools/instantspoolcreator.h"
#include "../checkers/maptracechecker.h"
#include "../checkers/lineartracechecker.h"
#include "../instantiation-tools/apsubbingcloner.h"

namespace texada {

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

    return mine_property_type(formula_string, trace_source, false, false, false);
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
    return mine_property_type(formula_string, trace_source, true, false, false);
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
set<const spot::ltl::formula*> mine_property_type(string formula_string,
        string trace_source, bool use_map, bool allow_reps,
        bool pregen_instants) {


    //parse the ltl formula
    spot::ltl::parse_error_list parse_errs;
    const spot::ltl::formula* formula = spot::ltl::parse(formula_string,
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
    spot::ltl::atomic_prop_set * variables = spot::ltl::atomic_prop_collect(
            formula);
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
    if (!vector_trace_set->empty()) {
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
    std::cout << "I lvoe me some segfaults \n";
    delete instantiator;
    std::cout << "I lvoe me some segfaults \n";
    delete variables;
    std::cout << "I lvoe me some segfaults \n";
    formula->destroy();
    return return_set;

}

} /* namespace texada */
