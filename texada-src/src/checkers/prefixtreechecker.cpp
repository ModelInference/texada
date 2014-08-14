/*
 * prefixtreechecker.cpp
 *
 *  Created on: Jul 18, 2014
 *      Author: clemieux
 */

#include "prefixtreechecker.h"


namespace texada {

prefix_tree_checker::prefix_tree_checker() {
    // TODO Auto-generated constructor stub

}

prefix_tree_checker::~prefix_tree_checker() {
    // TODO Auto-generated destructor stub
}


/**
 * Since SPOT's visitor only takes in one parameter (the formula), the accept functionality
 * cannot be used. Thus we need a function that takes in a general formula and correctly
 * assigns which helper should deal with it.
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
bool prefix_tree_checker::check_on_single_trace(const spot::ltl::formula* form_node, shared_ptr<prefix_tree_node> trace_node, int id) {
    switch (form_node->kind()) {
    case spot::ltl::formula::Constant:
        return check_on_single_trace(static_cast<const spot::ltl::constant*>(form_node), trace_node, id);
    case spot::ltl::formula::AtomicProp:
        return check_on_single_trace(static_cast<const spot::ltl::atomic_prop*>(form_node), trace_node,id);
    case spot::ltl::formula::UnOp:
        return check_on_single_trace(static_cast<const spot::ltl::unop*>(form_node), trace_node, id);
    case spot::ltl::formula::BinOp:
        return check_on_single_trace(static_cast<const spot::ltl::binop*>(form_node), trace_node, id);
    case spot::ltl::formula::MultOp:
        return check_on_single_trace(static_cast<const spot::ltl::multop*>(form_node), trace_node, id);
    case spot::ltl::formula::BUnOp:
        return check_on_single_trace(static_cast<const spot::ltl::bunop*>(form_node));
    case spot::ltl::formula::AutomatOp:
        return check_on_single_trace(static_cast<const spot::ltl::automatop*>(form_node));
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
inline bool prefix_tree_checker::check_on_single_trace(const spot::ltl::atomic_prop *form_node, shared_ptr<prefix_tree_node> trace_node, int id) {
    return (trace_node->get_name() == form_node->name()) ? true : false;
}

/**
 * spot::ltl::constant is either true, false, or empty word. On true, we return true
 * and on false we return false. Checking an empty word on a trace should probably
 * be true (since if the formula expresses nothing, it should vacuously hold true).
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
bool prefix_tree_checker::check_on_single_trace(const spot::ltl::constant *form_node, shared_ptr<prefix_tree_node> trace_node, int id ){
    spot::ltl::constant::type value = form_node->val();
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
bool prefix_tree_checker::check_on_single_trace(const spot::ltl::binop *form_node, shared_ptr<prefix_tree_node> trace_node, int id) {
    spot::ltl::binop::type opkind = form_node->op();

    switch (opkind) {

    //XOR case: if first is true, return true if second is false,
    //if first is false, return true if second is true.
    case spot::ltl::binop::Xor:
        return (check_on_single_trace(form_node->first(), trace_node, id)) ?
                !(check_on_single_trace(form_node->second(), trace_node, id)) : check_on_single_trace(form_node->second(), trace_node,id);

        //Implies case: if first is true, return true if second is true,
        //if first is false, return true.
    case spot::ltl::binop::Implies:
        return check_on_single_trace(form_node->first(), trace_node, id) ? check_on_single_trace(form_node->second(), trace_node, id) : true;

        //Equiv case: if first is true, return true if second is true,
        //if first is false, return true if second is false.
    case spot::ltl::binop::Equiv:
        return check_on_single_trace(form_node->first(), trace_node, id) ?
                check_on_single_trace(form_node->second(), trace_node, id) : !check_on_single_trace(form_node->second(), trace_node, id);

        //Until case
    case spot::ltl::binop::U:
        //if we get here, we did not see second: thus, false
        if (trace_node->is_terminal()) {
            return false;
        }
        // if the second condition is satisfied and we have gotten here,
        // the first condition held all the way up here, so true
        else if (check_on_single_trace(form_node->second(), trace_node, id)) {
            return true;
        }
        // if the first condition does not hold before the second, false
        else if (!check_on_single_trace(form_node->first(), trace_node, id)) {
            return false;
        }
        // if the first condition holds, check on the next suffix trace
        else {
            return check_on_single_trace(form_node, trace_node->get_child(id), id);
        }

        //Release case
    case spot::ltl::binop::R:
        //if we get here, second always held: true
        if (trace_node->is_terminal()) {
            return true;
        }
        // if the f & s is satisfied and we have gotten here,
        // the second condition held all the way up here, so true
        else if (check_on_single_trace(form_node->second(), trace_node, id) && check_on_single_trace(form_node->first(), trace_node, id)) {
            return true;
        }

        // if the second condition does not hold before f & s, false
        else if (!check_on_single_trace(form_node->second(), trace_node, id)) {
            return false;
        }
        // if the second condition holds, check on the next suffix trace
        else {
            return check_on_single_trace(form_node, trace_node->get_child(id), id);
        }

        //Weak until case: identical to until except base case
    case spot::ltl::binop::W:
        //if we get here, we did not see second and first was true throughout
        if (trace_node->is_terminal()) {
            return true;
        }
        // if the second condition is satisfied and we have gotten here,
        // the first condition held all the way up here, so true
        else if (check_on_single_trace(form_node->second(), trace_node, id)) {
            return true;
        }
        // if the first condition does not hold before the second, false
        else if (!check_on_single_trace(form_node->first(), trace_node, id)) {
            return false;
        }
        // if the first condition holds, check on the next suffix trace
        else {
            return check_on_single_trace(form_node, trace_node->get_child(id), id);
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
bool prefix_tree_checker::check_on_single_trace(const spot::ltl::unop *form_node, shared_ptr<prefix_tree_node> trace_node, int id) {

    spot::ltl::unop::type optype = form_node->op();

    //TODO: refactor node->child into a variable

    switch (optype) {

    // Globally case
    case spot::ltl::unop::G:
        // base case: if we're at END_VAR, return true to not effect &&
        if (trace_node->is_terminal()) {
            return true;
        } else {
            //Return whether subformula is true on this trace, recursive check on
            // all subsequent traces.
            return check_on_single_trace(form_node->child(), trace_node, id) && check_on_single_trace(form_node, trace_node->get_child(id), id);
        }

        // Finally case
    case spot::ltl::unop::F:
        // base case: if we're at END_VAR, return false to not effect ||
        if (trace_node->is_terminal()) {
            return false;
        } else {
            //Return whether subformula is true on this trace, recursive check on
            // all subsequent traces.
            return check_on_single_trace(form_node->child(), trace_node, id) || check_on_single_trace(form_node, trace_node->get_child(id), id);
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
        if (trace_node->is_terminal()) {
            return check_on_single_trace(form_node->child(), trace_node, id);
        }
        return check_on_single_trace(form_node->child(), trace_node->get_child(id), id);

        // Not case
    case spot::ltl::unop::Not:
        return !check_on_single_trace(form_node->child(), trace_node, id);

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
bool prefix_tree_checker::check_on_single_trace(const spot::ltl::multop* form_node, shared_ptr<prefix_tree_node> trace_node, int id) {
    spot::ltl::multop::type opkind = form_node->op();

    switch (opkind) {

    case spot::ltl::multop::Or: {
        int numkids = form_node->size();
        // for each of the multop's children, we check if it is true: if it is,
        // we short circuit and return true. If we have not returned by the
        // end of the loop, then none of the children were true and we return
        // false.
        for (int i = 0; i < numkids; i++) {
            if (check_on_single_trace(form_node->nth(i), trace_node, id)) {
                return true;
            }
        }
        return false;
    }
    case spot::ltl::multop::And: {
        int numkids = form_node->size();
        // for each of the multop's children, we check if it is false: if it is,
        // we short circuit and return false. If we have not returned by the
        // end of the loop, then none of the children were false and we return
        // true.
        for (int i = 0; i < numkids; i++) {
            if (!check_on_single_trace(form_node->nth(i), trace_node, id)) {
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
 * Gets the next node if this is a prefix tree node.
 * @param current_node
 * @return
 */
map<set<int>,linear_trace_checker::trace_node> prefix_tree_checker::get_next_event(const trace_node current_node){
    trace_node return_node;
    map<set<int>,trace_node> return_map;
    set<int> return_set;
    return_set.insert(0);
    // TODO: stub for version w/o memoization
    get<shared_ptr<prefix_tree_node>>(return_node) = get<shared_ptr<prefix_tree_node>>(current_node)->get_child(1);
    return_map.emplace(return_set,return_node);
    return return_map;
}

/**
 * Returns true if the current node is terminal
 * @param current_node current position in trace
 * @return is this a terminal event
 */
bool prefix_tree_checker::is_terminal(const trace_node current_node){
    return get<const string_event*>(current_node)->is_terminal();
}

/**
 * Returns name of the current event
 * @param current_node current position in trace
 * @return is this a terminal event
 */
string prefix_tree_checker::event_name(const trace_node current_node){
    return get<const string_event*>(current_node)->get_name();
}

} /* namespace texada */
