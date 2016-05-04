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
#include <time.h>

#include <boost/program_options.hpp>

#include "../parsers/parser.h"
#include "../parsers/translationsparser.h"
#include "../parsers/linearparser.h"
#include "../parsers/mapparser.h"
#include "../parsers/prefixtreeparser.h"
#include "../instantiation-tools/constinstantspool.h"
#include "../instantiation-tools/scopedinstantspoolcreator.h"
#include "../instantiation-tools/pregeninstantspool.h"
#include "../instantiation-tools/otfinstantspool.h"
#include "../instantiation-tools/instantspoolcreator.h"
#include "../checkers/maptracechecker.h"
#include "../checkers/lineartracechecker.h"
#include "../checkers/prefixtreechecker.h"
#include "../checkers/statistic.h"
#include "../checkers/settings.h"
#include "../instantiation-tools/apsubbingcloner.h"
#include "opts.h"

namespace texada {


/**
 * Finds instantiations of the given property type using the linear
 * trace checker. NOTE: this method is only used in testing.
 * @param formula_string the string form of the property type/
 * ltl formula to be mined
 * @param trace_source the input source of the trace
 * @return valid instantiations of the property type on the trace
 */
vector<vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> mine_lin_property_type(string formula_string,
        string trace_source) {
    return mine_property_type(
            set_options("-f '" + formula_string + "' -l " + trace_source));
}

/**
 * Finds instantiations of the given property type using the map
 * trace checker. NOTE: this method is only used in testing.
 * @param formula_string the string form of the property type/
 * ltl formula to be mined
 * @param trace_source the input source of the trace
 * @return valid instantiations of the property type on the trace
 */
vector<vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> mine_map_property_type(string formula_string,
        string trace_source) {
    return mine_property_type(
            set_options("-f '" + formula_string + "' -m " + trace_source));
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
vector<vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> mine_property_type(
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
    // whether to generate with scope sematics
    bool scope_semantics = opts.count("scope-semantics");

    /*
     * Setting support, support-potential, and confidence thresholds.
     * By default: sup_threshold = 0; sup_pot_threshold = 0; conf_threshold = 1.00
     */
    settings c_settings;

    // whether to print statistics
    c_settings.compute_full_stats = opts.count("print-stats");

    // whether inputed thresholds are global
    c_settings.use_global_t = opts.count("use-global-thresholds");

    if (opts.count("no-vacuous-findings")) {
        c_settings.set_sup_t(1);
        c_settings.use_global_t = true;
    }
    if (opts.count("sup-threshold")) {
        c_settings.set_sup_t(opts["sup-threshold"].as<int>());
    }
    if (opts.count("sup-pot-threshold")) {
        c_settings.set_sup_pot_t(opts["sup-pot-threshold"].as<int>());
    }
    if (opts.count("conf-threshold")) {
        c_settings.set_conf_t(opts["conf-threshold"].as<float>());
    }

    bool use_invariant_semantics = false;
    shared_ptr<map<string,string>> translations = nullptr;
#ifdef SMT_SUPPORT
    // Setting options for invariant semantics
    if (opts.count("invariant-semantics-input")){
       use_invariant_semantics = true;
       translations_parser t_parser = translations_parser();
       std::ifstream t_infile(opts["invariant-semantics-input"].as<string>());
       t_parser.parse(t_infile);
       translations = t_parser.get_translations();
    }
#endif

    // currently, only the vanilla configuration is supported for
    // the map and prefix checkers. So, stop program if a non-vanilla
    // configuration is called with a non-linear checker
    if (!use_lin && !c_settings.is_vanilla()) {
        std::cerr << "Statistics-related options were called for a non-linear checker. "
                "Currently, the map and prefix checkers do not support statistic-related options."<< "\n";
        exit(1);
    }

    // the property type
    vector<string> prop_types = opts["property-type"].as<std::vector<std::string>>();
    // trace source
    string trace_source = opts["log-file"].as<std::string>();


    // if any constant events are specified in command line,
    // take these into count
    vector<string> constant_events;
    if (opts.count("event")) {
        constant_events = opts["event"].as<vector<string>>();
    }

    //parse the ltl formulas
    spot::ltl::parse_error_list parse_errs;
    vector<const spot::ltl::formula *> formulae;
    for (unsigned int i = 0; i < prop_types.size(); i++){
        formulae.push_back(spot::ltl::parse(prop_types[i], parse_errs));
        assert(parse_errs.size() == 0);
    }
    // parse file
    std::ifstream infile(trace_source);
    if (infile.fail()) {
        std::cerr << "Error: could not open file \n";
        return vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>>();
    }

    parser * parser;
    if (use_map) {
        parser = new map_parser();
    } else if (use_lin) {
        parser = new linear_parser();
    } else if (use_pretree) {
        parser = new prefix_tree_parser();
    } else {
        std::cerr << "Trace type not specified, nothing gets parsed.";
        exit(1);
    }
    // configure parser based on user options
    if (opts.count("regex")) {
        parser->set_prop_types(opts["regex"].as<vector<string>>());
    }
    if (opts.count("trace-separator")) {
        parser->set_trace_separator(opts["trace-separator"].as<std::string>());
    }
    if (opts.count("parse-mult-prop")) {
        parser->parse_mult_prop();
    }
    if (opts.count("ignore-nm-lines")) {
        parser->ignore_nm_lines();
    }
    parser->parse(infile);

    shared_ptr<set<string>> event_set = parser->return_props();

    // make different set of events to exclude for each formula
    vector<vector<string>> exclude_event_sets;
    for (unsigned int i = 0; i < formulae.size(); i++){
        exclude_event_sets.push_back(vector<string>());
    }

    // create the set of formulas' variables
    vector<shared_ptr<spot::ltl::atomic_prop_set>> variable_vec;

    for (unsigned int i = 0; i < formulae.size(); i++){
        variable_vec.push_back(shared_ptr<spot::ltl::atomic_prop_set>(spot::ltl::atomic_prop_collect(formulae[i])));
    }

    // add events which should be removed from the event set for each formula
    if (!allow_reps && (constant_events.size() > 0)) {
        // loop through the constant events
        for (int i = constant_events.size() - 1; i >= 0; i--) {
            // loop through each formula
            for (unsigned int j = 0; j < formulae.size(); j++){
                // loop through variables for each formula
                for (set<const spot::ltl::atomic_prop*>::iterator it = variable_vec[j]->begin();
                        it != variable_vec[j]->end(); it++){
                    // add variable to events to be excluded if it appears in the formula
                    if ((*it)->name() == constant_events[i]){
                        exclude_event_sets[j].push_back(constant_events[i]);
                        break;
                    }
                }
            }
        }
    }


    // remove variables which are specified as constant events
    for (unsigned int i = 0; i <variable_vec.size(); i++){
        if (constant_events.size() > 0) {
            spot::ltl::atomic_prop_set::iterator it = variable_vec[i]->begin();
            while (it != variable_vec[i]->end()) {
                bool erase = false;
                for (unsigned int i = 0; i < constant_events.size(); i++) {
                    if ((*it)->name() == constant_events.at(i)) {
                        erase = true;
                    }
                }
                if (erase) {
                    spot::ltl::atomic_prop_set::iterator toErase = it;
                    ++it;
                    variable_vec[i]->erase(toErase);
                } else {
                    ++it;
                }
            }
        }
    }

    // create the instantiator
    instants_pool_creator * instantiator;
    // check everything
    vector<vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> valid_instants_vec;
    for (unsigned int i = 0; i < formulae.size(); i++){
        // remove events to be excluded for this formula, if any
        for (unsigned int j = 0; j < exclude_event_sets[i].size(); j++){
            event_set->erase(exclude_event_sets[i].at(j));
        }
        // create the instantiators
        if (variable_vec[i]->empty()) {
            instantiator = new const_instants_pool(formulae[i]);
        } else if (pregen_instants) {
            instantiator = new pregen_instants_pool(event_set, variable_vec[i],
                    allow_reps, constant_events);
        } else if (scope_semantics){
            instantiator = new scoped_instants_pool_creator(event_set, variable_vec[i],
                    allow_reps, constant_events);
        } else {
            instantiator = new otf_instants_pool(event_set,  variable_vec[i], allow_reps,
                    constant_events);
        }

        // check all valid instantiations on each trace
        if (use_lin) {
            shared_ptr<std::multiset<vector<event> >> vector_trace_set =
                    dynamic_cast<linear_parser*>(parser)->return_vec_trace();
            valid_instants_vec.push_back(valid_instants_on_traces(formulae[i], instantiator,
                    vector_trace_set, c_settings, use_invariant_semantics, translations));
        } else if (use_map) {
            shared_ptr<set<map<event, vector<long>>> > map_trace_set = dynamic_cast<map_parser*>(parser)->return_map_trace();
            valid_instants_vec.push_back(valid_instants_on_traces(formulae[i], instantiator,
                    map_trace_set, use_invariant_semantics, translations));
        } else if (use_pretree) {
            shared_ptr<prefix_tree> prefix_tree_traces = dynamic_cast<prefix_tree_parser*>(parser)->return_prefix_trees();
            valid_instants_vec.push_back(valid_instants_on_traces(formulae[i], instantiator,
                    prefix_tree_traces, use_invariant_semantics, translations));
        }

        // delete instantiator
        delete instantiator;

        // restore events that were excluded for this formula
        for (unsigned int j = 0; j < exclude_event_sets[i].size(); j++){
            event_set->insert(exclude_event_sets[i].at(j));
        }

       }


    delete parser;
    for (int i = formulae.size() - 1 ; i >= 0; i--){
        formulae[i]->destroy();
    } // might not be ok
    return valid_instants_vec;

}

} /* namespace texada */
