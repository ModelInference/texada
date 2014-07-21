/*
 * lineartracechecker.cpp
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 */

#include "lineartracechecker.h"
#include "../instantiation-tools/apsubbingcloner.h"

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
        const string_event *trace) {
    switch (node->kind()) {
    case spot::ltl::formula::Constant:
        return check(static_cast<const spot::ltl::constant*>(node), trace);
    case spot::ltl::formula::AtomicProp:
        return check(static_cast<const spot::ltl::atomic_prop*>(node), trace);
    case spot::ltl::formula::UnOp:
        return check(static_cast<const spot::ltl::unop*>(node), trace);
    case spot::ltl::formula::BinOp:
        return check(static_cast<const spot::ltl::binop*>(node), trace);
    case spot::ltl::formula::MultOp:
        return check(static_cast<const spot::ltl::multop*>(node), trace);
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
        const string_event *trace) {
    return (trace[0].get_name() == node->name()) ? true : false;
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
        const string_event *trace) {
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
        const string_event *trace) {
    spot::ltl::binop::type opkind = node->op();

    switch (opkind) {

    //XOR case: if first is true, return true if second is false,
    //if first is false, return true if second is true.
    case spot::ltl::binop::Xor:
        return (check(node->first(), trace)) ?
                !(check(node->second(), trace)) : check(node->second(), trace);

        //Implies case: if first is true, return true if second is true,
        //if first is false, return true.
    case spot::ltl::binop::Implies:
        return check(node->first(), trace) ? check(node->second(), trace) : true;

        //Equiv case: if first is true, return true if second is true,
        //if first is false, return true if second is false.
    case spot::ltl::binop::Equiv:
        return check(node->first(), trace) ?
                check(node->second(), trace) : !check(node->second(), trace);

        //Until case
    case spot::ltl::binop::U:
        //if we get here, we did not see second: thus, false
        if (trace[0].is_terminal()) {
            return false;
        }
        // if the second condition is satisfied and we have gotten here,
        // the first condition held all the way up here, so true
        else if (check(node->second(), trace)) {
            return true;
        }
        // if the first condition does not hold before the second, false
        else if (!check(node->first(), trace)) {
            return false;
        }
        // if the first condition holds, check on the next suffix trace
        else {
            return check(node, trace + 1);
        }

        //Release case
    case spot::ltl::binop::R:
        //if we get here, second always held: true
        if (trace[0].is_terminal()) {
            return true;
        }
        // if the f & s is satisfied and we have gotten here,
        // the second condition held all the way up here, so true
        else if (check(node->second(), trace) && check(node->first(), trace)) {
            return true;
        }

        // if the second condition does not hold before f & s, false
        else if (!check(node->second(), trace)) {
            return false;
        }
        // if the second condition holds, check on the next suffix trace
        else {
            return check(node, trace + 1);
        }

        //Weak until case: identical to until except base case
    case spot::ltl::binop::W:
        //if we get here, we did not see second and first was true throughout
        if (trace[0].is_terminal()) {
            return true;
        }
        // if the second condition is satisfied and we have gotten here,
        // the first condition held all the way up here, so true
        else if (check(node->second(), trace)) {
            return true;
        }
        // if the first condition does not hold before the second, false
        else if (!check(node->first(), trace)) {
            return false;
        }
        // if the first condition holds, check on the next suffix trace
        else {
            return check(node, trace + 1);
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
        const string_event *trace) {

    spot::ltl::unop::type optype = node->op();

    //TODO: refactor node->child into a variable

    switch (optype) {

    // Globally case
    case spot::ltl::unop::G:
        // base case: if we're at END_VAR, return true to not effect &&
        if (trace[0].is_terminal()) {
            return true;
        } else {
            //Return whether subformula is true on this trace, recursive check on
            // all subsequent traces.
            return check(node->child(), trace) && check(node, trace + 1);
        }

        // Finally case
    case spot::ltl::unop::F:
        // base case: if we're at END_VAR, return false to not effect ||
        if (trace[0].is_terminal()) {
            return false;
        } else {
            //Return whether subformula is true on this trace, recursive check on
            // all subsequent traces.
            return check(node->child(), trace) || check(node, trace + 1);
        }

        // Next case
    case spot::ltl::unop::X:
        // Here we are effectively extending the end of the trace into an infinite
        // sequence of terminal variables.
        // We will never go past the end of the array:
        // if node->child() or any subsequent child is a propositional logic
        // operation, constant or atomic proposition, we don't advance to trace+1.
        // if node->child() or any subsequent child is F, G, U, R or W then
        // the first base case (if (trace[0].is_terminal())) is evaluated
        // and a boolean is returned
        // if node->child() or any subsequent child is X, we enter the base case
        // below, remaining in the array.
        if (trace[0].is_terminal()) {
            return check(node->child(), trace);
        }
        return check(node->child(), trace + 1);

        // Not case
    case spot::ltl::unop::Not:
        return !check(node->child(), trace);

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
        const string_event *trace) {
    spot::ltl::multop::type opkind = node->op();

    switch (opkind) {

    case spot::ltl::multop::Or: {
        int numkids = node->size();
        // for each of the multop's children, we check if it is true: if it is,
        // we short circuit and return true. If we have not returned by the
        // end of the loop, then none of the children were true and we return
        // false.
        for (int i = 0; i < numkids; i++) {
            if (check(node->nth(i), trace)) {
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
            if (!check(node->nth(i), trace)) {
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
 * Checks whether the given instantitations of a formula hold on the
 * given trace -- if they do not, the validity of the function is set
 * to false. The original instantiation array is the one returned.
 * @param instantiations all instantiation function mappings
 * @param formula the LTL formula to instantiate
 * @param trace the trace to check on
 * @return updated instantiations, with invalid ones set to false
 */
shared_ptr<vector<pregen_instants_pool::inst_fxn>> linear_trace_checker::check_instants_on_trace(
        shared_ptr<vector<pregen_instants_pool::inst_fxn>> instantiations,
        const spot::ltl::formula* formula, const string_event* trace) {
    int size = instantiations->size();
    //#pragma omp parallel for shared(instantiations)
    for (int i = 0; i < size; i++) {
        // if it's invalid, ignore
        if (!(instantiations->at(i).valid))
            continue;
        map<string, string> current_map = instantiations->at(i).inst_map;
        const spot::ltl::formula* instantiated_form = instantiate(formula,
                current_map);
        instantiations->at(i).valid = check(instantiated_form, trace);
        instantiated_form->destroy();
    }
    //hi
    return instantiations;

}

} /* namespace texada */
