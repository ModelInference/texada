/*
 * lineartracechecker.cpp
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 */

#include "lineartracechecker.h"
#include "boolbasedchecker.h"
#include "../instantiation-tools/apsubbingcloner.h"
#include "../instantiation-tools/pregeninstantspool.h"
#include "ltlvisit/tostring.hh"
#include "ltlvisit/simplify.hh"
#include "tgba/bdddict.hh"
#include "statistic.h"
#include "settings.h"
namespace texada {

statistic linear_trace_checker::check_on_trace(const spot::ltl::formula * node,
        const string_event * trace) {
    return this->check(node, trace);
}

/**
 * Checking a single event on a trace means we check it on the first element of the trace
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
statistic linear_trace_checker::ap_check(const spot::ltl::atomic_prop *node,
        const string_event *trace, std::set<int> trace_ids) {
    if (trace->get_name() == node->name()) {
        return statistic(true, 1, 1);
    } else {
        return statistic(false, 0, 1);
    }
}

/**
 * p U q will be true if !p does not occur before (exclusive) q
 * Also, q must occur.
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::until_check(const spot::ltl::binop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();

    statistic stat_p;
    statistic stat_q;

    //if we get here, we did not see q: thus, false
    if (trace_pt->is_terminal()) {
        return statistic(false, 0, 1);  // Dennis: not entirely sure about this
    }
    // if the q holds here, we have not yet seen q or !p, (these
    //  cause return) so true
    else if ((stat_q = this->check(q, trace_pt)).is_satisfied) {
        return stat_q;
    }
    // we know q does not hold from above, so if p does not hold,
    // we have !p and !q, which violates p U q.
    else if (is_short_circuiting(stat_p = this->check(p, trace_pt))) {
        return stat_p;
    }
    // if !q and p holds, check on the next suffix trace
    else {
        return statistic(stat_p, this->check(node, trace_pt + 1));
    }
}

/**
 * p R q will be true if !q does not occur before (inclusive) q
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::release_check(const spot::ltl::binop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();

    statistic stat_p;
    statistic stat_q;

    //if we get here, q always held: true
    if (trace_pt->is_terminal()) {
        return statistic(true, 0, 0);   // Dennis: not entirely sure about this
    }

    // if !q occurs before p & q, false
    else if (is_short_circuiting(stat_q = this->check(q, trace_pt))) {
        return stat_q;
    }

    // we know from the previous if that q holds and held up to here,
    // so if p also holds, return true
    else if ((stat_p = this->check(p, trace_pt)).is_satisfied) {
        return stat_q;
    }

    // if the q holds, check on the next suffix trace
    else {
        return statistic(stat_q, this->check(node, trace_pt + 1));
    }

}

/**
 * p M q will be true if !q does not occur before (inclusive) q. q must occur.
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::strongrelease_check(const spot::ltl::binop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();

    statistic stat_p;
    statistic stat_q;

    //if we get here, q always held, p never occurred: false
    if (trace_pt->is_terminal()) {
        return statistic(false, 0, 1);
    }
    // if !q occurs before p & q, false
    else if (is_short_circuiting(stat_q = this->check(q, trace_pt))) {
        return stat_q;
    }

    // we know from the previous if that q holds and held up to here,
    // so if p also holds, return true
    else if ((stat_p = this->check(p, trace_pt)).is_satisfied) {
        return statistic(stat_q, stat_p);
    }

    // if the q holds, check on the next suffix trace
    else {
        return statistic(stat_q, this->check(node, trace_pt + 1));
    }

}

/**
 * p W q will be true if !p does not occur before (exclusive) q
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::weakuntil_check(const spot::ltl::binop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();

    statistic stat_p;
    statistic stat_q;

    //if we get here, we did not see q or !p, so true.
    if (trace_pt->is_terminal()) {
        return statistic(true, 0, 0);
    }
    // if the q holds here, we have not yet seen q or !p, (these
    //  cause return) so true
    else if ((stat_q = this->check(q, trace_pt)).is_satisfied) {
        return statistic(true, 0, 0);
    }
    // we know q does not hold from above, so if p does not hold,
    // we have !p and !q, which violates p U q.
    else if (is_short_circuiting(stat_p = this->check(p, trace_pt))) {
        return stat_p;
    }
    // if !q and p holds, check on the next suffix trace
    else {
        return statistic(stat_p, this->check(node, trace_pt + 1));
    }


}


/**
 * Gp will be true if Gp is true on every trace suffix of the given
 * trace, i.e. if p holds at all time
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::globally_check(const spot::ltl::unop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->child();

    statistic stat_p;

    // base case: if we're at END_VAR, return true to not effect &&
    if (trace_pt->is_terminal()) {
        return statistic(true, 0, 0);
    } else if (is_short_circuiting(stat_p = this->check(p, trace_pt))) {
        return stat_p;
    } else {
        //Return whether subformula is true on this trace, recursive check on
        // all subsequent traces.
        return statistic(stat_p, this->check(node, trace_pt + 1));
    }
}


/**
 * Fp will be true if Fp is true on some trace suffix of the given
 * trace, i.e. if p holds at some time
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::finally_check(const spot::ltl::unop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->child();

    statistic stat_p;

    // base case: if we're at END_VAR, return false to not effect ||
    if (trace_pt->is_terminal()) {
        return statistic(false, 0, 1);
    } else if ((stat_p = this->check(p, trace_pt)).is_satisfied) {
        return stat_p;
    } else {
        //Return whether subformula is true on this trace, recursive check on
        // all subsequent traces.
        return this->check(node, trace_pt + 1);
    }
}


/**
 * Xp will be true if p is true on the next suffix of the given
 * trace, i.e. if p holds at the next time
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::next_check(const spot::ltl::unop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->child();
    // if we are at the terminal event, the next event is also a terminal
    // event. Since we are traversing a finite tree, this will terminate.
    if (trace_pt->is_terminal()) {
        return this->check(p, trace_pt);
    }
    return this->check(p, trace_pt + 1);
}

/**
 * Finds valid instants on a given set of traces using a linear checker
 * configured to the default (i.e. "vanilla") setting.
 * @param prop_type property type to check.
 * @param instantiator instantiator to produce next instantiation
 * @param traces all the traces to check on
 * @return
 */
vector<std::pair<map<string, string>, statistic>> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<std::multiset<vector<string_event>>> traces) {
    return valid_instants_on_traces(prop_type, instantiator, traces, settings());
}

/**
 * Using custom configuration of the linear checker, check all
 * instantiations of the property type given on the traces and return
 * the ones which are valid (according to the specified checker settings).
 * @param prop_type property type to check.
 * @param instantiator instantiator to produce next instantiation
 * @param traces all the traces to check on
 * @param c_settings user-specified configuration for checker
 * @return
 */
vector<std::pair<map<string, string>, statistic>> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<std::multiset<vector<string_event>>> traces,
        settings c_settings) {
    instantiator->reset_instantiations();
    // vector to return
    vector<std::pair<map<string, string>, statistic>> return_vec;
    linear_trace_checker checker;
    // set checker configurations
    checker.configure(c_settings);
    // simplifier for turning formulas into negative normal form so that
    // statistics of formulae involving not, xor, <-> can be computed.
    std::unique_ptr<spot::ltl::ltl_simplifier> simplifier(new spot::ltl::ltl_simplifier());

    // Loop through each instantiation, filtering out those which are invalid (the notion of invalidity
    // depending on the given checker settings).
    // Note that it would be nice to output the number of instantiations which were found to be true
    // on the trace set but failed to satisfy given statistical thresholds and were subsequently filtered out.
    // This however would require certain short-circuiting to be turned off, and so the current implementation
    // does not support this feature.
    while (true) {
        shared_ptr<map<string,string>> current_instantiation = instantiator->get_next_instantiation();
        if (current_instantiation == NULL) {
            break;
        }
        const spot::ltl::formula * instantiated_prop_type = instantiate(prop_type,*current_instantiation,
                instantiator->get_events_to_exclude());
        // unless checker is configured for the vanilla setting, turn formula into negative normal form.
        // note that the the operators xor, <->, and -> will be reduced to basic operators by the below code.
        if (!c_settings.is_vanilla()) {
            // move original formula to a temp ptr to be destroyed
            const spot::ltl::formula * to_delete = instantiated_prop_type;
            instantiated_prop_type = simplifier->negative_normal_form(instantiated_prop_type);
            to_delete->destroy();
        }
        // the meaning of validity will depend on the user inputed settings
        bool valid = true;
        statistic global_stat = statistic(true, 0, 0);
        for (auto each = traces->begin(); each != traces->end(); each=traces->upper_bound(*each)) {
            statistic trace_stat = checker.check_on_trace(instantiated_prop_type,&(each->at(0)));
            // when there are multiple equivalent traces, the instantiation is only checked over one
            // of them. To compute the correct statistics, multiply the statistics for each trace by
            // the number of "occurrences" of said trace in the log.
            int trace_count = traces->count(*each);
            trace_stat.support = trace_stat.support * trace_count;
            trace_stat.support_potential = trace_stat.support_potential * trace_count;
            global_stat = statistic(global_stat, trace_stat);

            // if confidence threshold is 1.0, an instantiation becomes invalid on the first unsatisfied trace,
            // so we short-circuit after flagging the current instantiation as invalid.
            if (c_settings.conf_t == 1.0 && !trace_stat.is_satisfied) {
                valid = false;
                break;
            }
            // in non-global setting, an instantiation becomes invalid on the first trace where a threshold is
            // unsatisfied, so we short-circuit after flagging the current instantiation as invalid.
            if (!c_settings.use_global_t && !c_settings.satisfies_thresholds(trace_stat)) {
                valid = false;
                break;
            }
            // in global 0-conf threshold setting, an instantiation becomes valid on the full trace set once
            // every threshold have been satisfied, so we short-circuit after flagging the current instantiation as valid.
            if (c_settings.use_global_t && c_settings.conf_t == 0.0) {
                if (!c_settings.compute_full_stats && c_settings.satisfies_thresholds(global_stat)) {
                    valid = true;
                    break;
                }

            }
        }
        // in the global setting, we need to check that the final statistics (i.e. the sum of statistics computed
        // at each trace) satisfy the given thresholds.
        if (c_settings.use_global_t) {
            valid = c_settings.satisfies_thresholds(global_stat);
        }

        instantiated_prop_type->destroy();
        // return finding if it is valid and its statistics meets all specified thresholds
        if (valid) {
            std::pair<map<string, string>, statistic> finding(*current_instantiation, global_stat);
            return_vec.push_back(finding);
        }
    }
    return return_vec;
}

}
/* namespace texada */
