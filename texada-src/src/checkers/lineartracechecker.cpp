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
#include "statistic.h"
#include "finding.h"
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
    statistic result;
    result.is_satisfied = (trace->get_name() == node->name()) ? true : false;
    // result.support = (result.is_satisfied) ? 1 : 0;
    // result.support_potential = 1;
    return result;
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
    statistic result;
    statistic result_p;
    statistic result_q;
    statistic result_next;

    //if we get here, we did not see q: thus, false
    if (trace_pt->is_terminal()) {
        result = statistic(false, 0, 1); // {TODO} should there be support potential returned from a terminal event?
        return result;
    }
    // if the q holds here, we have not yet seen q or !p, (these
    //  cause return) so true
    else if ((result_q = this->check(q, trace_pt)).is_satisfied) {
        return result_q;  // {TODO}
    }
    // we know q does not hold from above, so if p does not hold,
    // we have !p and !q, which violates p U q.
    // if !q and p holds, check on the next suffix trace
    else {
        result_p = this->check(p, trace_pt);
        result_next = this->check(node, trace_pt + 1);
        result = statistic(result_p, result_next);
        return result;
    }
}

/**
 * p R q will be true if !p does not occur before (inclusive) q
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::release_check(const spot::ltl::binop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();
    statistic result;
    statistic result_p;
    statistic result_q;
    statistic result_next;

    //if we get here, q always held: true
    if (trace_pt->is_terminal()) {
        result = statistic(true, 0, 0); // Dennis: or should it be {true, 1, 1}?
        return result;
    }

    // we know from the previous if that q holds and held up to here,
    // so if p also holds, return true
    else if ((result_p = this->check(p, trace_pt)).is_satisfied) {
        result_q = this->check(q, trace_pt);
        result = statistic(result_q, result_p);
        return result;
    }

    // we know from above that p has failed to be released, so we check
    // if q holds here and check recursively down the trace
    else {
        result_q = this->check(q, trace_pt);
        result_next = this->check(node, trace_pt + 1);
        result = statistic(result_q, result_next);
        return result;                 // Dennis: consider passing in the partial result into the check function to achieve tail recursion
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
    statistic result;
    statistic result_p;
    statistic result_q;
    statistic result_next;

    //if we get here, q always held, p never occurred: false
    if (trace_pt->is_terminal()) {
        result = statistic(false, 0, 1); // Dennis: or should it be {true, 0, 0}?
        return result;
    }

    // we know from the previous if that q holds and held up to here,
    // so if p also holds, return true
    else if ((result_p = this->check(p, trace_pt)).is_satisfied) {
        result_q = this->check(q, trace_pt);
        result = statistic(result_q, result_p);
        return result;
    }

    // if the q holds, check on the next suffix trace
    else {
        result_q = this->check(q, trace_pt);
        result_next = this->check(node, trace_pt + 1);
        result = statistic(result_q, result_next);
        return result;                 // Dennis: consider passing in the partial result into the check function to achieve tail recursion
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
    statistic result;
    statistic result_p;
    statistic result_q;
    statistic result_next;

    //if we get here, we did not see q or !p, so true.
    if (trace_pt->is_terminal()) {
        result = statistic(true, 0, 0);
        return result;
    }
    // if the q holds here, we have not yet seen q or !p, (these
    //  cause return) so true
    else if ((result_q = this->check(q, trace_pt)).is_satisfied) {
        return result_q;
    }
    // we know q does not hold from above, so if p does not hold,
    // we have !p and !q, which violates p U q.
    // if !q and p holds, check on the next suffix trace
    else {
        result_p = this->check(p, trace_pt);
        result_next = this->check(node, trace_pt + 1);
        result = statistic(result_p, result_next);
        return result;
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
    statistic result;
    statistic result_p;
    statistic result_next;

    // base case: if we're at END_VAR, return true to not effect &&
    if (trace_pt->is_terminal()) {
        result = {true, 0, 0};
        return result;
    } else {
        //Return whether subformula is true on this trace, recursive check on
        // all subsequent traces.
        result_p = this->check(p, trace_pt);
        result_next = this->check(node, trace_pt + 1);
        result = statistic(result_p, result_next);
        return result;
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
    statistic result;
    statistic result_p;

    // base case: if we're at END_VAR, return false to not effect ||
    if (trace_pt->is_terminal()) {
        result = statistic(false, 0, 1);         // Dennis: should it be {false, 0, 0}
        return result;
    } else if ((result_p = this->check(p, trace_pt)).is_satisfied) {
        // if p has been satisfied, return the result of checking p
        return result_p;
    } else {
        // if p has not been satisfied, recursively check on subsequent traces
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
statistic linear_trace_checker::next_check(const spot::ltl::unop* node,  // Dennis: compute and return sup and sup_pot;  change return type to return support and confidence (or support potential?)
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
 * Check all instantiations of the property type given on the traces
 * and return the ones which are valid (i.e. uses default confidence of 100)
 * @param prop_type property type to check.
 * @param instantiator instantiator to produce next instantiation
 * @param traces all the traces to check on
 * @return
 */
vector<finding> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<set<vector<string_event>>> traces) {
    return valid_instants_on_traces(prop_type, instantiator, traces, 0, 0, 1.0, false, false);
}

/**
 * Check all instantiations of the property type given on the traces
 * and return the ones having confidence above the inputed threshold
 * @param prop_type property type to check.
 * @param instantiator instantiator to produce next instantiation
 * @param traces all the traces to check on
 * @param conf_threshold the confidence threshold used to filter findings
 * @return
 */
vector<finding> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<set<vector<string_event>>> traces,
        int sup_t,
        int sup_pot_t,
        float conf_t,
        bool global,
        bool print_stats) {
            instantiator->reset_instantiations();
            // vector to return
            vector<finding> return_vec;
            linear_trace_checker checker;
            // set checker thresholds
            checker.configure(sup_t, sup_pot_t, conf_t, print_stats);
            while (true) {
                shared_ptr<map<string,string>> current_instantiation = instantiator->get_next_instantiation();
                if (current_instantiation == NULL) {
                    break;
                }
                const spot::ltl::formula * instantiated_prop_type = instantiate(prop_type,*current_instantiation,
                instantiator->get_events_to_exclude());
                bool valid = true;
                statistic result = statistic(true, 0, 0);
                for (set<vector<string_event>>::iterator traces_it = traces->begin();
                traces_it != traces->end(); traces_it++) {
                    statistic result_i = checker.check_on_trace(instantiated_prop_type,&(traces_it->at(0)));
                    result = statistic(result, result_i);
                    // in vanilla option, short-circuit on first unsatisfied trace
                    if (conf_t == 1.0 && !result_i.is_satisfied) {
                        valid = false;
                        break;
                    }
                    // in !!! option, short-circuit on first trace where threshold is unsatisfied
                    if (!global && (result_i.support < sup_t || result_i.support_potential < sup_pot_t)) {
                        valid = false;
                        break;
                    }
                    // in !!! option, short-circuit once thresholds are satisfied
                    if (global && conf_t == 0.0 && !print_stats && (result.support >= sup_t && result.support_potential >= sup_pot_t)) {
                        valid = true;
                        break;
                    }
                }
                instantiated_prop_type->destroy();
                // calculate confidence of result
                // int result_conf = (result.support_potential != 0) ? (result.support / result.support_potential) * 100 : 0;   // Dennis: by default, should we remove vacuously true findings? Should conf be measured out of 1 or 100?
                //if (result_conf >= conf_threshold) {
                //if (result.support >= sup_t && result.support_potential >= sup_pot_t) {
                if (valid) {
                    finding f = {*current_instantiation, result};
                    return_vec.push_back(f);
                }
            }
            return return_vec;

        }

    }
    /* namespace texada */
