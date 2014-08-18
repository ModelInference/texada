/*
 * prefixtreechecker.cpp
 *
 *  Created on: Jul 18, 2014
 *      Author: clemieux
 */

#include "prefixtreechecker.h"
#include "../instantiation-tools/apsubbingcloner.h"

#include <ltlvisit/tostring.hh>

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
bool prefix_tree_checker::check_on_single_trace(
        const spot::ltl::formula* form_node,
        shared_ptr<prefix_tree_node> trace_node, int id) {
    switch (form_node->kind()) {
    case spot::ltl::formula::Constant:
        return check_on_single_trace(
                static_cast<const spot::ltl::constant*>(form_node), trace_node,
                id);
    case spot::ltl::formula::AtomicProp:
        return check_on_single_trace(
                static_cast<const spot::ltl::atomic_prop*>(form_node),
                trace_node, id);
    case spot::ltl::formula::UnOp:
        return check_on_single_trace(
                static_cast<const spot::ltl::unop*>(form_node), trace_node, id);
    case spot::ltl::formula::BinOp:
        return check_on_single_trace(
                static_cast<const spot::ltl::binop*>(form_node), trace_node, id);
    case spot::ltl::formula::MultOp:
        return check_on_single_trace(
                static_cast<const spot::ltl::multop*>(form_node), trace_node,
                id);
    case spot::ltl::formula::BUnOp:
        return check_on_single_trace(
                static_cast<const spot::ltl::bunop*>(form_node));
    case spot::ltl::formula::AutomatOp:
        return check_on_single_trace(
                static_cast<const spot::ltl::automatop*>(form_node));
    default:
        return false;
    }
}

//TODO:Comment.....
bool prefix_tree_checker::check_on_trace(
        const spot::ltl::formula* form_node,
        shared_ptr<prefix_tree_node> trace_node, map<string,string> instantiations_){
    use_memo = true;
    instantiations = instantiations_;
    return check_on_trace(form_node, trace_node);
}

/**
 * Checking a single event on a trace means we check it on the first element of the trace
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
inline bool prefix_tree_checker::check_on_single_trace(
        const spot::ltl::atomic_prop *form_node,
        shared_ptr<prefix_tree_node> trace_node, int id) {
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
bool prefix_tree_checker::check_on_single_trace(
        const spot::ltl::constant *form_node,
        shared_ptr<prefix_tree_node> trace_node, int id) {
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
bool prefix_tree_checker::check_on_single_trace(
        const spot::ltl::binop *form_node,
        shared_ptr<prefix_tree_node> trace_node, int id) {
    spot::ltl::binop::type opkind = form_node->op();

    switch (opkind) {

    //XOR case: if first is true, return true if second is false,
    //if first is false, return true if second is true.
    case spot::ltl::binop::Xor:
        return (check_on_single_trace(form_node->first(), trace_node, id)) ?
                !(check_on_single_trace(form_node->second(), trace_node, id)) :
                check_on_single_trace(form_node->second(), trace_node, id);

        //Implies case: if first is true, return true if second is true,
        //if first is false, return true.
    case spot::ltl::binop::Implies:
        return check_on_single_trace(form_node->first(), trace_node, id) ?
                check_on_single_trace(form_node->second(), trace_node, id) :
                true;

        //Equiv case: if first is true, return true if second is true,
        //if first is false, return true if second is false.
    case spot::ltl::binop::Equiv:
        return check_on_single_trace(form_node->first(), trace_node, id) ?
                check_on_single_trace(form_node->second(), trace_node, id) :
                !check_on_single_trace(form_node->second(), trace_node, id);

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
            return check_on_single_trace(form_node, trace_node->get_child(id),
                    id);
        }

        //Release case
    case spot::ltl::binop::R:
        //if we get here, second always held: true
        if (trace_node->is_terminal()) {
            return true;
        }
        // if the f & s is satisfied and we have gotten here,
        // the second condition held all the way up here, so true
        else if (check_on_single_trace(form_node->second(), trace_node, id)
                && check_on_single_trace(form_node->first(), trace_node, id)) {
            return true;
        }

        // if the second condition does not hold before f & s, false
        else if (!check_on_single_trace(form_node->second(), trace_node, id)) {
            return false;
        }
        // if the second condition holds, check on the next suffix trace
        else {
            return check_on_single_trace(form_node, trace_node->get_child(id),
                    id);
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
            return check_on_single_trace(form_node, trace_node->get_child(id),
                    id);
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
bool prefix_tree_checker::check_on_single_trace(
        const spot::ltl::unop *form_node,
        shared_ptr<prefix_tree_node> trace_node, int id) {

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
            return check_on_single_trace(form_node->child(), trace_node, id)
                    && check_on_single_trace(form_node,
                            trace_node->get_child(id), id);
        }

        // Finally case
    case spot::ltl::unop::F:
        // base case: if we're at END_VAR, return false to not effect ||
        if (trace_node->is_terminal()) {
            return false;
        } else {
            //Return whether subformula is true on this trace, recursive check on
            // all subsequent traces.
            return check_on_single_trace(form_node->child(), trace_node, id)
                    || check_on_single_trace(form_node,
                            trace_node->get_child(id), id);
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
        return check_on_single_trace(form_node->child(),
                trace_node->get_child(id), id);

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
bool prefix_tree_checker::check_on_single_trace(
        const spot::ltl::multop* form_node,
        shared_ptr<prefix_tree_node> trace_node, int id) {
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
 * Top-level check function that consolidates branch results.
 * @param node formula to check
 * @param trace_pt beginning of trace/prefix tree
 * @return
 */
bool prefix_tree_checker::check_on_trace(const spot::ltl::formula* node,
        const trace_node trace_pt) {
    set<int> set = trace_pt->get_trace_ids();
    map<int, bool> branch_results = this->check(node, trace_pt, set);
    for (map<int, bool>::iterator result_it = branch_results.begin();
            result_it != branch_results.end(); result_it++) {
        if (!result_it->second) {
            return false;
        }
    }
    return true;

}

/**
 * Explores all specified branches with node.
 * @param node
 * @param trace_pts the children of the previous branch point
 * @param trace_ids the branches to check
 * @return map of evaluation values.
 */
map<int, bool> prefix_tree_checker::check_on_kids(
        const spot::ltl::formula* node, map<set<int>, trace_node> kids,
        set<int> trace_ids) {
    // return map
    map<int, bool> return_map;
    // start iterating through the kids
    for (map<set<int>, trace_node>::iterator kids_it = kids.begin();
            kids_it != kids.end(); kids_it++) {
        // intersection of this kid's traces and the traces we want to pursue
        set<int> intersect;
        std::set_intersection(trace_ids.begin(), trace_ids.end(),
                kids_it->first.begin(), kids_it->first.end(),
                std::inserter(intersect, intersect.begin()));
        // if there's a branch we want to pursue in kid, go check down.
        // only check branches we want to check.
        if (intersect.size() != 0) {
            map<int, bool> result_on_kid = this->check(node, kids_it->second,
                    intersect);
            return_map.insert(result_on_kid.begin(), result_on_kid.end());

        }

    }
    return return_map;
}

/**
 * Return a false map
 * @param trace_ids
 * @return
 */
map<int, bool> prefix_tree_checker::false_check(std::set<int> trace_ids) {
    return create_int_bool_map(trace_ids, false);
}
/**
 * Return a true map
 * @param trace_ids
 * @return
 */
map<int, bool> prefix_tree_checker::true_check(std::set<int> trace_ids) {
    return create_int_bool_map(trace_ids, true);
}

/**
 * Checking a single event on a trace means we check it on the first element of the trace
 * @param node: the atomic proposition to check
 * @param trace_pt: pointer to the start of the trace
 * @param trace_ids: trace ids we're checking on
 * @return whether node holds on trace
 */
map<int, bool> prefix_tree_checker::ap_check(const spot::ltl::atomic_prop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    if (use_memo){
        if (instantiations.find(node->name()) != instantiations.end()){
            bool is_this_event = (instantiations.find(node->name()))->second == trace_pt->get_name();
            return create_int_bool_map(trace_ids, is_this_event);
        } else
        {
            std::cout << "SHOULD NOT GET HERE \n";
            return create_int_bool_map(trace_ids,false);
        }
    } else {
    // evaluate whether the AP holds
    bool is_this_event = (trace_pt->get_name() == node->name()) ? true : false;
    // return in trace_id -> bool map
    return create_int_bool_map(trace_ids, is_this_event);
    }
}

/**
 *  p xor q. if p is true, return true if q is false,
 *  if p is false, return true if q is true.
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::xor_check(const spot::ltl::binop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();
    // xor requires checking both p and q, no shortcuts here
    map<int, bool> qholds = this->check(q, trace_pt, trace_ids);
    map<int, bool> pholds = this->check(p, trace_pt, trace_ids);
    for (map<int, bool>::iterator pholds_it = pholds.begin();
            pholds_it != pholds.end(); pholds_it++) {
        bool qholds_on_trace = qholds.find(pholds_it->first)->second;
        pholds_it->second =
                (pholds_it->second) ? !qholds_on_trace : qholds_on_trace;

    }
    return pholds;
}

/**
 *  p <-> q. if p is true, return true if q is true,
 *  if p is false, return true.
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::equiv_check(const spot::ltl::binop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();
    // <-> requires checking both p and q, no shortcuts here
    map<int, bool> qholds = this->check(q, trace_pt, trace_ids);
    map<int, bool> pholds = this->check(p, trace_pt, trace_ids);
    for (map<int, bool>::iterator pholds_it = pholds.begin();
            pholds_it != pholds.end(); pholds_it++) {
        bool qholds_on_trace = qholds.find(pholds_it->first)->second;
        pholds_it->second =
                (pholds_it->second) ? qholds_on_trace : !qholds_on_trace;

    }
    return pholds;

}

/**
 * p -> q if p is true, return true if q is true. If p is false, return true.
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::implies_check(const spot::ltl::binop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();
    map<int, bool> return_map;

    map<int, bool> pholds = this->check(p, trace_pt, trace_ids);
    // where p is false we don't need to recurse. Add those
    // pairs and remove trace_ids where !p.
    add_satisfying_values(pholds, false, return_map, trace_ids);
    // we've added false when this actually makes p -> q true,
    // so not all the values:
    return_map = not_map(return_map);
    // if some traces return p, then the value of p -> q is
    // the value of q on this point.
    if (trace_ids.size() != 0) {
        map<int, bool> qholds = this->check(q, trace_pt, trace_ids);
        return_map.insert(qholds.begin(), qholds.end());
    }
    return return_map;
}

/**
 * p U q: p must be true until the first occurrence of q, q must occur
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::until_check(const spot::ltl::binop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();
    map<int, bool> return_map;
    //if we get here, we did not see q: thus, false
    if (trace_pt->is_terminal()) {
        return create_int_bool_map(trace_ids, false);
    }
    // if the q holds here, we have not yet seen q or !p, (these
    //  cause return) so true values are true overall. retain
    // the true values and remove the trace ids with true values
    // from trace_ids
    map<int, bool> q_holds = this->check(q, trace_pt, trace_ids);
    add_satisfying_values(q_holds, true, return_map, trace_ids);
    // if q held on all traces, return
    if (trace_ids.size() == 0) {
        return return_map;
    }

    // we know q does not hold from above, so if p does not hold,
    // we have !p before q, which violates p U q.
    map<int, bool> p_holds = this->check(p, trace_pt, trace_ids);
    // where !p, !(p U q), so insert as false:
    add_satisfying_values(p_holds, false, return_map, trace_ids);
    // if we've determined values on all traces, return
    if (trace_ids.size() == 0) {
        return return_map;
    }

    // if !q and p holds, check on the next suffix trace
    map<int, bool> pUq_on_kids = check_on_kids(node, trace_pt->get_children(),
            trace_ids);
    // those values are the value of pUq on here
    return_map.insert(pUq_on_kids.begin(), pUq_on_kids.end());
    return return_map;

}

/**
 * p R q: q must be true up to and including the first occurrence of p
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::release_check(const spot::ltl::binop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();
    map<int, bool> return_map;
    //if we get here, q was always true, so true
    if (trace_pt->is_terminal()) {
        return create_int_bool_map(trace_ids, true);
    }
    // if the !q holds here, since we haven't yet seen p,
    // p R q is false (!q occurred before p)
    map<int, bool> q_holds = this->check(q, trace_pt, trace_ids);
    add_satisfying_values(q_holds, false, return_map, trace_ids);
    // if !q held on all traces, return
    if (trace_ids.size() == 0) {
        return return_map;
    }

    // traces remaining in trace_ids where ones where q held, so
    // if p holds, p & q holds:
    map<int, bool> p_holds = this->check(p, trace_pt, trace_ids);
    // where p, p & q, so p R q, so insert as true:
    add_satisfying_values(p_holds, true, return_map, trace_ids);
    // if we've determined values on all traces, return
    if (trace_ids.size() == 0) {
        return return_map;
    }

    // if !p and q, check on next suffix trace
    map<int, bool> pRq_on_kids = check_on_kids(node, trace_pt->get_children(),
            trace_ids);
    // those values are the value of pRq on here
    return_map.insert(pRq_on_kids.begin(), pRq_on_kids.end());
    return return_map;
}

/**
 * p W q: p must be true until the first occurrence of q,
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::weakuntil_check(
        const spot::ltl::binop* node, trace_node trace_pt,
        std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();
    map<int, bool> return_map;
    //if we get here, we did not see q: thus, false
    if (trace_pt->is_terminal()) {
        return create_int_bool_map(trace_ids, true);
    }
    // if the q holds here, we have not yet seen q or !p, (these
    //  cause return) so true values are true overall. retain
    // the true values and remove the trace ids with true values
    // from trace_ids
    map<int, bool> q_holds = this->check(q, trace_pt, trace_ids);
    add_satisfying_values(q_holds, true, return_map, trace_ids);
    // if q held on all traces, return
    if (trace_ids.size() == 0) {
        return return_map;
    }

    // we know q does not hold from above, so if p does not hold,
    // we have !p before q, which violates p W q.
    map<int, bool> p_holds = this->check(p, trace_pt, trace_ids);
    // where !p, !(p W q), so insert as false:
    add_satisfying_values(p_holds, false, return_map, trace_ids);
    // if we've determined values on all traces, return
    if (trace_ids.size() == 0) {
        return return_map;
    }

    // if !q and p holds, check on the next suffix trace
    map<int, bool> pWq_on_kids = check_on_kids(node, trace_pt->get_children(),
            trace_ids);
    // those values are the value of pWq on here
    return_map.insert(pWq_on_kids.begin(), pWq_on_kids.end());
    return return_map;

}

/**
 * p M q: q must be true up to and including the first occurrence of p.
 * p must occur
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::strongrelease_check(
        const spot::ltl::binop* node, trace_node trace_pt,
        std::set<int> trace_ids) {
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();
    map<int, bool> return_map;
    //if we get here, q was always true, so false
    if (trace_pt->is_terminal()) {
        return create_int_bool_map(trace_ids, false);
    }
    // if the !q holds here, since we haven't yet seen p,
    // p R q is false (!q occurred before p)
    map<int, bool> q_holds = this->check(q, trace_pt, trace_ids);
    add_satisfying_values(q_holds, false, return_map, trace_ids);
    // if !q held on all traces, return
    if (trace_ids.size() == 0) {
        return return_map;
    }

    // traces remaining in trace_ids where ones where q held, so
    // if p holds, p & q holds:
    map<int, bool> p_holds = this->check(p, trace_pt, trace_ids);
    // where p, p & q, so p M q, so insert as true:
    add_satisfying_values(p_holds, true, return_map, trace_ids);
    // if we've determined values on all traces, return
    if (trace_ids.size() == 0) {
        return return_map;
    }

    // if !p and q, check on next suffix trace
    map<int, bool> pMq_on_kids = check_on_kids(node, trace_pt->get_children(),
            trace_ids);
    // those values are the value of pRq on here
    return_map.insert(pMq_on_kids.begin(), pMq_on_kids.end());
    return return_map;
}

/**
 * Globally case: given Gp, p must hold on every point of the subsequent trace
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::globally_check(const spot::ltl::unop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    //renaming the child p
    const spot::ltl::formula * p = node->child();
    // create return map:
    map<int, bool> return_map;
    // base case: if we're at terminal, return true to not effect and behaviour
    if (trace_pt->is_terminal()) {
        return create_int_bool_map(trace_ids, true);
    } else {
        // check whether p is true
        map<int, bool> p_holds_here = this->check(p, trace_pt, trace_ids);
        // for those where p is false, add to return map and remove
        // from trace_ids to prevent recursion
        add_satisfying_values(p_holds_here, false, return_map, trace_ids);
        // add the values that were not truncated
        if (trace_ids.size() != 0) {
            map<int, bool> Fp_holds_on_rest = check_on_kids(node,
                    trace_pt->get_children(), trace_ids);
            return_map.insert(Fp_holds_on_rest.begin(), Fp_holds_on_rest.end());
        }
        return return_map;
    }
}

/**
 * Finally case: given Fp, p must hold on one of the subsequent trace points
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::finally_check(const spot::ltl::unop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    //renaming the child p
    const spot::ltl::formula * p = node->child();
    // create return map:
    map<int, bool> return_map;
    // base case: if we're at terminal, return false to not effect or behavior
    if (trace_pt->is_terminal()) {
        return create_int_bool_map(trace_ids, false);
    } else {
        // check whether p is true
        map<int, bool> p_holds_here = this->check(p, trace_pt, trace_ids);
        // for those where p is true, add to return map and remove
        // from trace_ids to prevent recursion
        add_satisfying_values(p_holds_here, true, return_map, trace_ids);
        // add the values that were not truncated
        if (trace_ids.size() != 0) {
            map<int, bool> Fp_holds_on_rest = check_on_kids(node,
                    trace_pt->get_children(), trace_ids);
            return_map.insert(Fp_holds_on_rest.begin(), Fp_holds_on_rest.end());
        }
        return return_map;
    }

}

/**
 * Next case: given Xp, p must hold on the next trace
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::next_check(const spot::ltl::unop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    //renaming the child p
    const spot::ltl::formula * p = node->child();
    // if we are at the terminal event, the next event is also a terminal
    // event. Since we are traversing a finite tree, this will terminate.
    if (trace_pt->is_terminal()) {
        return this->check(p, trace_pt, trace_ids);
    }
    return check_on_kids(p, trace_pt->get_children(), trace_ids);

}

/**
 * Next case: given !p, p must not hold
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::not_check(const spot::ltl::unop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    //renaming the child p
    const spot::ltl::formula * p = node->child();
    return not_map(this->check(p, trace_pt, trace_ids));

}

/**
 * Or: true if one of or's children is true
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::or_check(const spot::ltl::multop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    // the return map;
    map<int, bool> return_map;
    int numkids = node->size();
    // for each of the or's children, we check if it is true: if it is,
    // we short circuit and return true. If we have not returned by the
    // end of the loop, then none of the children were true and we return
    // false.
    for (int i = 0; i < numkids; i++) {
        map<int, bool> kid_valids = this->check(node->nth(i), trace_pt,
                trace_ids);
        // add the children who are true, remove trace_ids we no longer need to check
        add_satisfying_values(kid_valids, true, return_map, trace_ids);
        // then continue if trace_ids is not empty
        if (trace_ids.size() == 0) {
            break;
        }
    }
    // if no kid was true on a trace, that trace_id will remain in trace_ids
    // and we will add false to the map
    for (set<int>::iterator it = trace_ids.begin(); it != trace_ids.end();
            it++) {
        return_map.emplace(*it, false);

    }
    return return_map;

}

/**
 * And: true if none of and's children are false
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, bool> prefix_tree_checker::and_check(const spot::ltl::multop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    // the return map;
    map<int, bool> return_map;
    int numkids = node->size();
    // for each of the and's children, we check if it is false: if it is,
    // we short circuit and return false. If we have not returned by the
    // end of the loop, then none of the children were false and we return
    // true.
    for (int i = 0; i < numkids; i++) {
        map<int, bool> kid_valids = this->check(node->nth(i), trace_pt,
                trace_ids);
        // add false children to return_map and remove those trace ids
        // from trace ids, thereby truncating evaluation.
        add_satisfying_values(kid_valids, false, return_map, trace_ids);
        // then continue if trace_ids is not empty
        if (trace_ids.size() == 0) {
            break;
        }
    }
    // if no kid was false on a trace, it will remain in trace_ids, so
    // put it in as true.
    for (set<int>::iterator it = trace_ids.begin(); it != trace_ids.end();
            it++) {
        return_map.emplace(*it, true);

    }
    return return_map;

}

/**
 * Creates an int-bool map with all the same bool value and keys given by ids
 * @param ids keys of map
 * @param valid value of map
 * @return
 */
map<int, bool> prefix_tree_checker::create_int_bool_map(set<int> ids,
        bool valid) {
    map<int, bool> return_map;
    for (set<int>::iterator it = ids.begin(); it != ids.end(); it++) {
        return_map.emplace(*it, valid);
    }
    return return_map;
}

/**
 * Returns the same map with all its values negatived
 * @param map to not
 * @return
 */
map<int, bool> prefix_tree_checker::not_map(map<int, bool> map) {
    for (std::map<int, bool>::iterator it = map.begin(); it != map.end();
            it++) {
        it->second = !it->second;
    }
    return map;
}

/**
 * Adds the pairs from returned_vals whose second is equal to to_satisfy
 * to map_to_return and the keys of the added pairs are removed from to_check.
 * @param returned_vals the values to filter
 * @param to_satisfy the value to truncate with
 * @param map_to_return map of return values for parent function
 * @param to_check the remaining traces to check; remove those for which
 * a truncating value has been found.
 */
void prefix_tree_checker::add_satisfying_values(map<int, bool>& returned_vals,
        bool to_satisfy, map<int, bool>& map_to_return, set<int>& to_check) {
    for (map<int, bool>::iterator it = returned_vals.begin();
            it != returned_vals.end(); it++) {
        if (it->second == to_satisfy) {
            map_to_return.insert(*it);
            to_check.erase(it->first);
        }
    }
}

void prefix_tree_checker::add_to_memo_map(const spot::ltl::atomic_prop* node,
        trace_node trace_pt, map<int,bool> return_val){
    memo_key insert_key;
    insert_key.node = node;
    insert_key.trace_pt = trace_pt;
    // STUB: TODO get correct bindings
    insert_key.relevant_mappings = map<string,string>();
    boost::unordered_map<memo_key, map<int,bool>, hash_memo_key>::iterator find_this = memo_map.find(insert_key);
    if (find_this != memo_map.end()){
        //todo: what can we infer about the value on unchecked traces???
        memo_map.emplace(insert_key, return_val);
    } else {
        for(map<int,bool>::iterator it = return_val.begin(); it != return_val.end() ;it++)
        find_this->second.insert(*it);
    }
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
        instants_pool_creator * instantiator, shared_ptr<prefix_tree> traces) {
    instantiator->reset_instantiations();
    // vector to return
    vector<map<string, string>> return_vec;
    prefix_tree_checker checker;
    set<shared_ptr<prefix_tree_node>> iterable_traces =
            traces->get_start_nodes();
    while (true) {
        shared_ptr<map<string, string>> current_instantiation =
                instantiator->get_next_instantiation();
        if (current_instantiation == NULL) {
            break;
        }
        const spot::ltl::formula * instantiated_prop_type = instantiate(
                prop_type, *current_instantiation,
                instantiator->get_events_to_exclude());
        // is the instantiation valid?
        bool valid = true;
        for (set<shared_ptr<prefix_tree_node>>::iterator it =
                iterable_traces.begin(); it != iterable_traces.end(); it++) {
            bool valid_on_trace = checker.check_on_trace(instantiated_prop_type,
                    *it);
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

} /* namespace texada */
