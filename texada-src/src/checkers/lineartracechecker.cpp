/*
 * lineartracechecker.cpp
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 */

#include "lineartracechecker.h"
#include "../instantiation-tools/apsubbingcloner.h"
#include "../instantiation-tools/pregeninstantspool.h"
namespace texada {


bool linear_trace_checker::check_on_trace(const spot::ltl::formula * node, const string_event * trace){
    return this->check(node, trace);
}

/**
 * Checking a single event on a trace means we check it on the first element of the trace
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
bool linear_trace_checker::ap_check(const spot::ltl::atomic_prop *node,
        const string_event *trace, std::set<int> trace_ids) {
    return (trace->get_name() == node->name()) ? true : false;
}

/**
 * p U q will be true if !p does not occur before (exclusive) q
 * Also, q must occur.
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
bool linear_trace_checker::until_check(const spot::ltl::binop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();

    //if we get here, we did not see q: thus, false
    if (trace_pt->is_terminal()) {
        return false;
    }
    // if the q holds here, we have not yet seen q or !p, (these
    //  cause return) so true
    else if (this->check(q, trace_pt)) {
        return true;
    }
    // we know q does not hold from above, so if p does not hold,
    // we have !p and !q, which violates p U q.
    else if (!this->check(p, trace_pt)) {
        return false;
    }
    // if !q and p holds, check on the next suffix trace
    else {
        return this->check(node, trace_pt + 1);
    }
}

/**
 * p R q will be true if !q does not occur before (inclusive) q
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
bool linear_trace_checker::release_check(const spot::ltl::binop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();

    //if we get here, q always held: true
    if (trace_pt->is_terminal()) {
        return true;
    }
    // if !q occurs before p & q, false
    else if (!this->check(q, trace_pt)) {
        return false;
    }

    // we know from the previous if that q holds and held up to here,
    // so if p also holds, return true
    else if (this->check(p, trace_pt)) {
        return true;
    }

    // if the q holds, check on the next suffix trace
    else {
        return this->check(node, trace_pt + 1);
    }

}

/**
 * p W q will be true if !p does not occur before (exclusive) q
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
bool linear_trace_checker::weakuntil_check(const spot::ltl::binop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();
    //if we get here, we did not see q or !p, so true.
    if (trace_pt->is_terminal()) {
        return true;
    }
    // if the q holds here, we have not yet seen q or !p, (these
    //  cause return) so true
    else if (this->check(q, trace_pt)) {
        return true;
    }
    // we know q does not hold from above, so if p does not hold,
    // we have !p and !q, which violates p U q.
    else if (!this->check(p, trace_pt)) {
        return false;
    }
    // if !q and p holds, check on the next suffix trace
    else {
        return this->check(node, trace_pt + 1);
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
bool linear_trace_checker::globally_check(const spot::ltl::unop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->child();
    // base case: if we're at END_VAR, return true to not effect &&
    if (trace_pt->is_terminal()) {
        return true;
    } else {
        //Return whether subformula is true on this trace, recursive check on
        // all subsequent traces.
        return this->check(p, trace_pt) && this->check(node, trace_pt + 1);
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
bool linear_trace_checker::finally_check(const spot::ltl::unop* node,
        const string_event* trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->child();
    // base case: if we're at END_VAR, return false to not effect ||
    if (trace_pt->is_terminal()) {
        return false;
    } else {
        //Return whether subformula is true on this trace, recursive check on
        // all subsequent traces.
        return this->check(p, trace_pt) || this->check(node, trace_pt + 1);
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
bool linear_trace_checker::next_check(const spot::ltl::unop* node,
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
 * and return the valid ones
 * @param prop_type property type to check.
 * @param instantiator instantiator to produce next instantiation
 * @param traces all the traces to check on
 * @return
 */
vector<map<string, string>> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<set<vector<string_event>>> traces) {
            instantiator->reset_instantiations();
            // vector to return
            vector<map<string, string>> return_vec;
            linear_trace_checker checker;
            while (true) {
                shared_ptr<map<string,string>> current_instantiation = instantiator->get_next_instantiation();
                if (current_instantiation == NULL) {
                    break;
                }
                const spot::ltl::formula * instantiated_prop_type = instantiate(prop_type,*current_instantiation,
                instantiator->get_events_to_exclude());
                // is the instantiation valid?
                bool valid = true;
                for (set<vector<string_event>>::iterator traces_it = traces->begin();
                traces_it != traces->end(); traces_it++) {
                    bool valid_on_trace = checker.check_on_trace(instantiated_prop_type,&(traces_it->at(0)));
                    if (!valid_on_trace) {
                        valid = false;
                        break;
                    }
                }
                instantiated_prop_type->destroy();
                if (valid) {
                    return_vec.push_back(*current_instantiation);
                }
            }
            return return_vec;

        }

    }
    /* namespace texada */
