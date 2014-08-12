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

linear_trace_checker::linear_trace_checker() {

}

linear_trace_checker::~linear_trace_checker() {

}

/**
 * Since SPOT's visitor only takes in one parameter (the formula), the accept functionality
 * cannot be used. Thus we need a function that takes in a general formula and correctly
 * assigns which helper should deal with it.
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
bool linear_trace_checker::check(const spot::ltl::formula* node,
        const trace_node trace_pt) {
    switch (node->kind()) {
    case spot::ltl::formula::Constant:
        return check(static_cast<const spot::ltl::constant*>(node), trace_pt);
    case spot::ltl::formula::AtomicProp:
        return check(static_cast<const spot::ltl::atomic_prop*>(node), trace_pt);
    case spot::ltl::formula::UnOp:
        return check(static_cast<const spot::ltl::unop*>(node), trace_pt);
    case spot::ltl::formula::BinOp:
        return check(static_cast<const spot::ltl::binop*>(node), trace_pt);
    case spot::ltl::formula::MultOp:
        return check(static_cast<const spot::ltl::multop*>(node), trace_pt);
    case spot::ltl::formula::BUnOp:
        return check(static_cast<const spot::ltl::bunop*>(node));
    case spot::ltl::formula::AutomatOp:
        return check(static_cast<const spot::ltl::automatop*>(node));
    default:
        return false;
    }
}

/**
 * Checking a single event on a trace means we check it on the first element of the trace
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
inline bool linear_trace_checker::check(const spot::ltl::atomic_prop *node,
        const trace_node trace_pt) {
    return (event_name(trace_pt) == node->name()) ? true : false;
}

/**
 * spot::ltl::constant is either true, false, or empty word. On true, we return true
 * and on false we return false. Checking an empty word on a trace should probably
 * be true (since if the formula expresses nothing, it should vacuously hold true).
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
bool linear_trace_checker::check(const spot::ltl::constant *node,
        const trace_node trace_pt) {
    spot::ltl::constant::type value = node->val();
    switch (value) {
    case spot::ltl::constant::True:
        return true;
    case spot::ltl::constant::False:
        return false;
    case spot::ltl::constant::EmptyWord:
        std::cerr
                << "We came across the empty word. Returning vacuously true. \n";
        return true;
    default:
        return false;
    }
}

/**
 * There are many binary operations. Each will be true for different reasons:
 * XOR: if first or second is true, but not both
 * Implies: if first is false, or both are true
 * Equiv: if both are false or both are true
 * Until: If second is true, return true. If it's not, check that first is:
 * if it's not, false, if it is, check next suffix trace. Second must occur.
 * Weak Until: As with until, but second must not occur
 * Release: first R second == second W (first & second)
 * Other operators are not supported
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
bool linear_trace_checker::check(const spot::ltl::binop *node,
        const trace_node trace_pt) {
    // find the operation kind of the given node
    spot::ltl::binop::type opkind = node->op();
    // name its first and second operands
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();
    // get the string_event*

    switch (opkind) {

    //XOR case: p xor q. if p is true, return true if q is false,
    //if p is false, return true if q is true.
    case spot::ltl::binop::Xor: {
        bool qholds = check(q, trace_pt);
        return (check(p, trace_pt)) ? !qholds : qholds;
    }
        //Implies case:  p -> q if p is true, return true if q is true,
        //if p is false, return true.
    case spot::ltl::binop::Implies: {
        return check(p, trace_pt) ? check(q, trace_pt) : true;
    }
        //Equiv case:  p <-> q if p is true, return true if q is true,
        //if p is false, return true if q is false.
    case spot::ltl::binop::Equiv:{
        bool qholds = check(q, trace_pt);
        return check(p, trace_pt) ?
               qholds : !qholds;
    }
        //Until case: p U q
    case spot::ltl::binop::U:{
        //if we get here, we did not see q: thus, false
        if (is_terminal(trace_pt)) {
            return false;
        }
        // if the q holds here, we have not yet seen q or !p, (these
        //  cause return) so true
        else if (check(q, trace_pt)) {
            return true;
        }
        // we know q does not hold from above, so if p does not hold,
        // we have !p and !q, which violates p U q.
        else if (!check(p, trace_pt)) {
            return false;
        }
        // if !q and p holds, check on the next suffix trace
        else {
            return check(node, get_next_event(trace_pt));
        }
    }
        //Release case: p R q
    case spot::ltl::binop::R:{
        //if we get here, q always held: true
        if (is_terminal(trace_pt)) {
            return true;
        }
        // if !q occurs before p & q, false
        else if (!check(q, trace_pt)) {
            return false;
        }

        // we know from the previous if that q holds and held up to here,
        // so if p also holds, return true
        else if (check(p, trace_pt)) {
            return true;
        }

        // if the q holds, check on the next suffix trace
        else {
            return check(node, get_next_event(trace_pt));
        }
    }

        //Weak until case: identical to until except base case
    case spot::ltl::binop::W:{
        //if we get here, we did not see q or !p, so true.
        if (is_terminal(trace_pt)) {
            return true;
        }
        // if the q holds here, we have not yet seen q or !p, (these
        //  cause return) so true
        else if (check(q, trace_pt)) {
            return true;
        }
        // we know q does not hold from above, so if p does not hold,
        // we have !p and !q, which violates p U q.
        else if (!check(p, trace_pt)) {
            return false;
        }
        // if !q and p holds, check on the next suffix trace
        else {
            return check(node, get_next_event(trace_pt));
        }
    }

    default:
        std::cerr << "Unsupported binary operator. Returning false. \n";
        return false;

    }
}

/**
 * A unary operation is either G, F, X or Not. SPOT also includes some other binary operators
 * which are not supported by this checker.
 * A formula which is globally true must be true on every trace suffix of the given trace.
 * A formula which is finally true must be true on some trace suffix of the given trace.
 * A formula which is next true must be true on the next trace suffix of the given trace.
 * A formula is not true is its negation holds on the current trace.
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
bool linear_trace_checker::check(const spot::ltl::unop *node,
        const trace_node trace_pt) {
    // the operation type
    spot::ltl::unop::type optype = node->op();
    //renaming the child p
    const spot::ltl::formula * p = node->child();

    //TODO: refactor node->child into a variable

    switch (optype) {

    // Globally case: Gp
    case spot::ltl::unop::G:
        // base case: if we're at END_VAR, return true to not effect &&
        if (is_terminal(trace_pt)) {
            return true;
        } else {
            //Return whether subformula is true on this trace, recursive check on
            // all subsequent traces.
            return check(p, trace_pt) && check(node, get_next_event(trace_pt));
        }

        // Finally case: Fp
    case spot::ltl::unop::F:
        // base case: if we're at END_VAR, return false to not effect ||
        if (is_terminal(trace_pt)) {
            return false;
        } else {
            //Return whether subformula is true on this trace, recursive check on
            // all subsequent traces.
            return check(p, trace_pt) || check(node, get_next_event(trace_pt));
        }

        // Next case: Xp
    case spot::ltl::unop::X:
        // if we are at the terminal event, the next event is also a terminal
        // event. Since we are traversing a finite tree, this will terminate.
        if (is_terminal(trace_pt)) {
            return check(p, trace_pt);
        }
        return check(p, get_next_event(trace_pt));

        // Not case: !p
    case spot::ltl::unop::Not:
        return !check(p, trace_pt);

        // Other operators are not LTL, don't support them
    default:
        std::cerr << "Unsupported unary operator. Returning false. \n";
        return false;

    }

}

/**
 * Multop includes or and and. SPOT also has other multops which are not supported.
 * Each element of the and needs to be true on the trace; only one element of the
 * or must be true on the trace.
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on the trace
 */
bool linear_trace_checker::check(const spot::ltl::multop* node,
        const trace_node trace_pt) {
    // is this an and or an or
    spot::ltl::multop::type opkind = node->op();

    switch (opkind) {

    case spot::ltl::multop::Or: {
        int numkids = node->size();
        // for each of the multop's children, we check if it is true: if it is,
        // we short circuit and return true. If we have not returned by the
        // end of the loop, then none of the children were true and we return
        // false.
        for (int i = 0; i < numkids; i++) {
            if (check(node->nth(i), trace_pt)) {
                return true;
            }
        }
        return false;
    }
    case spot::ltl::multop::And: {
        int numkids = node->size();
        // for each of the multop's children, we check if it is false: if it is,
        // we short circuit and return false. If we have not returned by the
        // end of the loop, then none of the children were false and we return
        // true.
        for (int i = 0; i < numkids; i++) {
            if (!check(node->nth(i), trace_pt)) {
                return false;
            }
        }
        return true;
    }
    default:
        std::cerr << "Unsupported multiple operator. Returning false. \n";
        return false;

    }

}

/**
 * Gets the next node if this is a linear trace node.
 * @param current_node
 * @return
 */
linear_trace_checker::trace_node linear_trace_checker::get_next_event(const trace_node current_node){
    trace_node return_node;
    get<const string_event*>(return_node) = get<const string_event*>(current_node) + 1;
    return return_node;
}

/**
 * Returns true if the current node is terminal
 * @param current_node current position in trace
 * @return is this a terminal event
 */
bool linear_trace_checker::is_terminal(const trace_node current_node){
    return get<const string_event*>(current_node)->is_terminal();
}

/**
 * Returns name of the current event
 * @param current_node current position in trace
 * @return is this a terminal event
 */
string linear_trace_checker::event_name(const trace_node current_node){
    return get<const string_event*>(current_node)->get_name();
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
                    linear_trace_checker::trace_node current_trace;
                    boost::get<const string_event*>(current_trace) = &(traces_it->at(0));
                    bool valid_on_trace = checker.check(instantiated_prop_type, current_trace);
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
