/*
 * lineartracechecker.h
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 */

#ifndef LINEARTRACECHECKER_H_
#define LINEARTRACECHECKER_H_

#include <ltlast/allnodes.hh>
#include "../trace/event.h"
#include "boolbasedchecker.h"
#include <iostream>
#include "statistic.h"
#include "settings.h"

#include "../instantiation-tools/pregeninstantspool.h"

namespace texada {


/**
 * Class to check whether an LTL formula is true on a trace in
 * flinear form. Can also be configured to retrieve support and
 * support potential statistics for an LTL formula on a trace.
 *
 */
class linear_trace_checker : public bool_based_checker<const event*>{
public:
	linear_trace_checker() {};
	virtual ~linear_trace_checker() {};

	statistic check_on_trace(const spot::ltl::formula* node, const event *trace);

private:

	virtual statistic ap_check(const spot::ltl::atomic_prop* node,
	            const event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic until_check(const spot::ltl::binop* node,
            const event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic release_check(const spot::ltl::binop* node,
            const event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic strongrelease_check(const spot::ltl::binop* node,
                const event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic weakuntil_check(const spot::ltl::binop* node,
            const event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic globally_check(const spot::ltl::unop* node,
            const event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic finally_check(const spot::ltl::unop* node,
            const event* trace_pt, std::set<int> trace_ids = std::set<int>());
    virtual statistic next_check(const spot::ltl::unop* node,
            const event* trace_pt, std::set<int> trace_ids = std::set<int>());

    // use invariant semantics
    bool use_invariant_semantics;
    shared_ptr<map<string,string>> translations;

};

/**
 * Finds valid instants on a given set of traces using a linear checker
 * configured to the following default (i.e. "vanilla") setting:
 * sup_threshold = 0
 * sup_pot_threshold = 0
 * conf_threshold = 1.0
 * global_thresholds = false
 * print_stats = false
 */
vector<std::pair<map<string, string>, statistic>> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<std::multiset<vector<event>>> traces,
        bool use_invar_semantics = false,
        shared_ptr<map<string,string>> translations = nullptr);

/**
 * Finds valid instants on traces based on given configuration
 */
vector<std::pair<map<string, string>, statistic>> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<std::multiset<vector<event>>> traces,
        settings c_settings,
        bool use_invar_semantics = false,
        shared_ptr<map<string,string>> translations = nullptr);

} /* namespace texada */

#endif /* LINEARTRACECHECKER2_H_ */
