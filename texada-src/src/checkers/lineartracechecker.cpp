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
    return (trace->get_name() == node->name()) ? statistic(true, 1, 1) : statistic(false, 0, 1);
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

    statistic result_p;
    statistic result_q;

    //if we get here, we did not see q: thus, false
    if (trace_pt->is_terminal()) {
        return statistic(false, 0, 1);  // Dennis: not entirely sure about this
    }
    // if the q holds here, we have not yet seen q or !p, (these
    //  cause return) so true
    else if ((result_q = this->check(q, trace_pt)).is_satisfied) {
        return result_q;
    }
    // we know q does not hold from above, so if p does not hold,
    // we have !p and !q, which violates p U q.
    else if (is_short_circuiting(result_p = this->check(p, trace_pt))) {
        return result_p;
    }
    // if !q and p holds, check on the next suffix trace
    else {
        return statistic(result_p, this->check(node, trace_pt + 1));
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

    statistic result_p;
    statistic result_q;

    //if we get here, q always held: true
    if (trace_pt->is_terminal()) {
        return statistic(true, 0, 0);   // Dennis: not entirely sure about this
    }

    // if !q occurs before p & q, false
    else if (is_short_circuiting(result_q = this->check(q, trace_pt))) {
        return result_q;
    }

    // we know from the previous if that q holds and held up to here,
    // so if p also holds, return true
    else if ((result_p = this->check(p, trace_pt)).is_satisfied) {
        return result_q;
    }

    // if the q holds, check on the next suffix trace
    else {
        return statistic(result_q, this->check(node, trace_pt + 1));
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

    statistic result_p;
    statistic result_q;

    //if we get here, q always held, p never occurred: false
    if (trace_pt->is_terminal()) {
        return statistic(false, 0, 1);
    }
    // if !q occurs before p & q, false
    else if (is_short_circuiting(result_q = this->check(q, trace_pt))) {
        return result_q;
    }

    // we know from the previous if that q holds and held up to here,
    // so if p also holds, return true
    else if ((result_p = this->check(p, trace_pt)).is_satisfied) {
        return statistic(result_q, result_p);
    }

    // if the q holds, check on the next suffix trace
    else {
        return statistic(result_q, this->check(node, trace_pt + 1));
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

    statistic result_p;
    statistic result_q;

    //if we get here, we did not see q or !p, so true.
    if (trace_pt->is_terminal()) {
        return statistic(true, 0, 0);
    }
    // if the q holds here, we have not yet seen q or !p, (these
    //  cause return) so true
    else if ((result_q = this->check(q, trace_pt)).is_satisfied) {
        return statistic(true, 0, 0);
    }
    // we know q does not hold from above, so if p does not hold,
    // we have !p and !q, which violates p U q.
    else if (is_short_circuiting(result_p = this->check(p, trace_pt))) {
        return result_p;
    }
    // if !q and p holds, check on the next suffix trace
    else {
        return statistic(result_p, this->check(node, trace_pt + 1));
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

    statistic result_p;

    // base case: if we're at END_VAR, return true to not effect &&
    if (trace_pt->is_terminal()) {
        return statistic(true, 0, 0);
    } else if (is_short_circuiting(result_p = this->check(p, trace_pt))) {
        return result_p;
    } else {
        //Return whether subformula is true on this trace, recursive check on
        // all subsequent traces.
        return statistic(result_p, this->check(node, trace_pt + 1));
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

    statistic result_p;

    // base case: if we're at END_VAR, return false to not effect ||
    if (trace_pt->is_terminal()) {
        return statistic(false, 0, 1);
    } else if ((result_p = this->check(p, trace_pt)).is_satisfied) {
        return result_p;
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
 * A finding will be short circuiting if the checker is currently
 * configured to allow short circuiting and either the finding is
 * negative with a conf_threshold of 1.0, or all sup and sup-pot
 * thresholds have been exceeded with a conf_threshold of 0.0
 * @param finding: the statistic of a finding
 * @return whether to short-circuit
 */
bool linear_trace_checker::is_short_circuiting(statistic finding) {
    if (this->conf_threshold == 1.0 && !this->print_stats) {
    // short circuit for vanilla setting
        return (finding.support >= this->sup_threshold
                && finding.support_potential >= this->sup_pot_threshold
                && !finding.is_satisfied);
    } else if (this->conf_threshold == 0.0 && !this->print_stats) {
    // short circuit for 0-conf threshold setting
        return (finding.support >= this->sup_threshold
                && finding.support_potential >= this->sup_pot_threshold);
    } else {
    // otherwise, no short circuiting
        return false;
    }
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
                    // in vanilla setting, short-circuit on first unsatisfied trace
                    if (conf_t == 1.0 && !result_i.is_satisfied) {
                        valid = false;
                        break;
                    }
                    // in non-global setting, short-circuit on first trace where a threshold is unsatisfied
                    if (!global && (result_i.support < sup_t || result_i.support_potential < sup_pot_t)) {  // should confidence also be checked non-globally?
                        valid = false;
                        break;
                    }
                    // in global 0-conf threshold setting, short-circuit once thresholds are satisfied
                    if (global && conf_t == 0.0 && !print_stats && (result.support >= sup_t && result.support_potential >= sup_pot_t)) {
                        valid = true;
                        break;
                    }
                }
                instantiated_prop_type->destroy();
                // calculate confidence of result
                float result_conf = (result.support_potential != 0) ? ((float) result.support / (float) result.support_potential) : 1.0;   // Dennis: by default, should we remove vacuously true findings?
                // return finding if it is valid (the meaning of which depends on checker's configuration) and its statistics meets all specified thresholds
                if (valid && result_conf >= conf_t && result.support >= sup_t && result.support_potential >= sup_pot_t) {
                    finding f = {*current_instantiation, result};
                    return_vec.push_back(f);
                }
            }
            return return_vec;

        }

    }
    /* namespace texada */
