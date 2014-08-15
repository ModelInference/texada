/*
 * lineartracechecker.cpp
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 */

#include "lineartracechecker.h"
#include <ltlvisit/tostring.hh>
#include "../instantiation-tools/apsubbingcloner.h"
#include "../instantiation-tools/pregeninstantspool.h"

#include <algorithm>

namespace texada {

linear_trace_checker::linear_trace_checker() {

}

linear_trace_checker::~linear_trace_checker() {

}

/**
 * Top-level check function that consolidates branch results.
 * @param node formula to check
 * @param trace_pt beginning of trace/prefix tree
 * @return
 */
bool linear_trace_checker::check_on_trace(const spot::ltl::formula* node,
        const trace_node trace_pt) {
    map<int, bool> branch_results = check(node, trace_pt,
            get_trace_ids(trace_pt));
    for (map<int, bool>::iterator result_it = branch_results.begin();
            result_it != branch_results.end(); result_it++) {
        if (!result_it->second) {
            return false;
        }
    }
    return true;

}

/**
 * Since SPOT's visitor only takes in one parameter (the formula), the accept functionality
 * cannot be used. Thus we need a function that takes in a general formula and correctly
 * assigns which helper should deal with it.
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
map<int, bool> linear_trace_checker::check(const spot::ltl::formula* node,
        const trace_node trace_pt, set<int> trace_ids) {
    switch (node->kind()) {
    case spot::ltl::formula::Constant:
        return check(static_cast<const spot::ltl::constant*>(node), trace_pt,
                trace_ids);
    case spot::ltl::formula::AtomicProp:
        return check(static_cast<const spot::ltl::atomic_prop*>(node), trace_pt,
                trace_ids);
    case spot::ltl::formula::UnOp:
        return check(static_cast<const spot::ltl::unop*>(node), trace_pt,
                trace_ids);
    case spot::ltl::formula::BinOp:
        return check(static_cast<const spot::ltl::binop*>(node), trace_pt,
                trace_ids);
    case spot::ltl::formula::MultOp:
        return check(static_cast<const spot::ltl::multop*>(node), trace_pt,
                trace_ids);
    case spot::ltl::formula::BUnOp:
        return check(static_cast<const spot::ltl::bunop*>(node));
    case spot::ltl::formula::AutomatOp:
        return check(static_cast<const spot::ltl::automatop*>(node));
    default:
        return map<int, bool>();
    }
}

/**
 * Checking a single event on a trace means we check it on the first element of the trace
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
map<int, bool> linear_trace_checker::check(const spot::ltl::atomic_prop *node,
        const trace_node trace_pt, set<int> trace_ids) {
    // evaluate whether the AP holds
    bool is_this_event = (event_name(trace_pt) == node->name()) ? true : false;
    // return in trace_id -> bool map
    return create_int_bool_map(get_trace_ids(trace_pt), is_this_event);
}

/**
 * spot::ltl::constant is either true, false, or empty word. On true, we return true
 * and on false we return false. Checking an empty word on a trace should probably
 * be true (since if the formula expresses nothing, it should vacuously hold true).
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
map<int, bool> linear_trace_checker::check(const spot::ltl::constant *node,
        const trace_node trace_pt, set<int> trace_ids) {
    spot::ltl::constant::type value = node->val();
    set<int> this_trace_ids = get_trace_ids(trace_pt);

    switch (value) {
    case spot::ltl::constant::True: {
        return create_int_bool_map(this_trace_ids, true);
    }
    case spot::ltl::constant::False: {
        return create_int_bool_map(this_trace_ids, false);
    }
    case spot::ltl::constant::EmptyWord:
        std::cerr << "We came across the empty word. Returning vacuous map. \n";
        return map<int, bool>();
    default:
        return map<int, bool>();
    }
}

/**
 * Explores all specified branches with node.
 * @param node
 * @param trace_pts the children of the previous branch point
 * @param trace_ids the branches to check
 * @return map of evaluation values.
 */
map<int, bool> linear_trace_checker::check_on_kids(
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
            map<int, bool> result_on_kid = check(node, kids_it->second,
                    intersect);
            return_map.insert(result_on_kid.begin(), result_on_kid.end());
            break;

        }

    }
    return return_map;
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
map<int, bool> linear_trace_checker::check(const spot::ltl::binop *node,
        const trace_node trace_pt, set<int> trace_ids) {
    // find the operation kind of the given node
    spot::ltl::binop::type opkind = node->op();
    // name its first and second operands
    const spot::ltl::formula * p = node->first();
    const spot::ltl::formula * q = node->second();
    map<int, bool> return_map;

    switch (opkind) {
    //XOR case: p xor q. if p is true, return true if q is false,
    //if p is false, return true if q is true.
    case spot::ltl::binop::Xor: {
        map<int, bool> qholds = check(q, trace_pt, trace_ids);
        map<int, bool> pholds = check(p, trace_pt, trace_ids);
        for (map<int, bool>::iterator pholds_it = pholds.begin();
                pholds_it != pholds.end(); pholds_it++) {
            bool qholds_on_trace = qholds.find(pholds_it->first)->second;
            pholds_it->second =
                    (pholds_it->second) ? !qholds_on_trace : qholds_on_trace;

        }
        return pholds;
    }
        //Implies case:  p -> q if p is true, return true if q is true,
        //if p is false, return true.
    case spot::ltl::binop::Implies: {
        map<int, bool> pholds = check(p, trace_pt, trace_ids);
        // where p is false we don't need to recurse. Add those
        // pairs and remove trace_ids where !p.
        add_satisfying_values(pholds, false, return_map, trace_ids);
        // we've added false when this actually makes p -> q true,
        // so not all the values:
        return_map = not_map(return_map);
        // if some traces return p, then the value of p -> q is
        // the value of q on this point.
        if (trace_ids.size() != 0) {
            map<int, bool> qholds = check(q, trace_pt, trace_ids);
            return_map.insert(qholds.begin(), qholds.end());
        }
        return return_map;
    }
        //Equiv case:  p <-> q if p is true, return true if q is true,
        //if p is false, return true if q is false.
    case spot::ltl::binop::Equiv: {
        map<int, bool> qholds = check(q, trace_pt, trace_ids);
        map<int, bool> pholds = check(p, trace_pt, trace_ids);
        for (map<int, bool>::iterator pholds_it = pholds.begin();
                pholds_it != pholds.end(); pholds_it++) {
            bool qholds_on_trace = qholds.find(pholds_it->first)->second;
            pholds_it->second =
                    (pholds_it->second) ? qholds_on_trace : !qholds_on_trace;

        }
        return pholds;
    }
        //Until case: p U q
    case spot::ltl::binop::U: {
        //TODO: in until, etc, check needs to be used, not check_on_trace
        //if we get here, we did not see q: thus, false
        if (is_terminal(trace_pt)) {
            return create_int_bool_map(trace_ids, false);
        }
        // if the q holds here, we have not yet seen q or !p, (these
        //  cause return) so true values are true overall
        map<int, bool> q_holds = check(q, trace_pt, trace_ids);
        add_satisfying_values(q_holds, true, return_map, trace_ids);
        // if q held on all traces, return
        if (trace_ids.size() == 0) {
            return return_map;
        }
        // we know q does not hold from above, so if p does not hold,
        // we have !p and !q, which violates p U q.
        map<int, bool> p_holds = check(p, trace_pt, trace_ids);
        // where !p, !(p U q), so insert as false:
        add_satisfying_values(p_holds, false, return_map, trace_ids);
        // if we've determined values on all traces, return
        if (trace_ids.size() == 0) {
            return return_map;
        }

        // if !q and p holds, check on the next suffix trace
        map<int, bool> pUq_on_kids = check_on_kids(node,
                get_next_event(trace_pt), trace_ids);
        // those values are the value of pUq on here
        return_map.insert(pUq_on_kids.begin(), pUq_on_kids.end());
        return return_map;

    }

        //Weak until case: identical to until except base case
    case spot::ltl::binop::W: {
        //if we get here, we did not see q or !p: thus, true
        if (is_terminal(trace_pt)) {
            return create_int_bool_map(trace_ids, true);
        }

        // if the q holds here, we have not yet seen q or !p, (these
        //  cause return) so true values are true overall
        map<int, bool> q_holds = check(q, trace_pt, trace_ids);
        add_satisfying_values(q_holds, true, return_map, trace_ids);
        // if q held on all traces, return
        if (trace_ids.size() == 0) {
            return return_map;
        }
        // we know q does not hold from above, so if p does not hold,
        // we have !p and !q, which violates p U q.
        map<int, bool> p_holds = check(p, trace_pt, trace_ids);
        // where !p, !(p U q), so insert as false:
        add_satisfying_values(p_holds, false, return_map, trace_ids);
        // if we've determined values on all traces, return
        if (trace_ids.size() == 0) {
            return return_map;
        }

        // if !q and p holds, check on the next suffix trace
        map<int, bool> pUq_on_kids = check_on_kids(node,
                get_next_event(trace_pt), trace_ids);
        // those values are the value of pUq on here
        return_map.insert(pUq_on_kids.begin(), pUq_on_kids.end());
        return return_map;
    }

        //Release case: p R q
    case spot::ltl::binop::R: {
        //if we get here, q was always true, so true
        if (is_terminal(trace_pt)) {
            return create_int_bool_map(trace_ids, true);
        }
        // if the !q holds here, since we haven't yet seen p,
        // p R q is false (!q occurred before p)
        map<int, bool> q_holds = check(q, trace_pt, trace_ids);
        add_satisfying_values(q_holds, false, return_map, trace_ids);
        // if !q held on all traces, return
        if (trace_ids.size() == 0) {
            return return_map;
        }

        // traces remaining in trace_ids where ones where q held, so
        // if p holds, p & q holds:
        map<int, bool> p_holds = check(p, trace_pt, trace_ids);
        // where p, p & q, so p R q, so insert as true:
        add_satisfying_values(p_holds, true, return_map, trace_ids);
        // if we've determined values on all traces, return
        if (trace_ids.size() == 0) {
            return return_map;
        }

        // if !p and q, check on next suffix trace
        map<int, bool> pRq_on_kids = check_on_kids(node,
                get_next_event(trace_pt), trace_ids);
        // those values are the value of pUq on here
        return_map.insert(pRq_on_kids.begin(), pRq_on_kids.end());
        return return_map;
    }

    default:
        std::cerr << "Unsupported binary operator: " << opkind
                << ". Returning false. \n";
        return map<int, bool>();

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
map<int, bool> linear_trace_checker::check(const spot::ltl::unop *node,
        const trace_node trace_pt, set<int> trace_ids) {
    // the operation type
    spot::ltl::unop::type optype = node->op();
    //renaming the child p
    const spot::ltl::formula * p = node->child();
    // create return map:
    map<int, bool> return_map;

    //TODO: refactor node->child into a variable

    switch (optype) {

    // Next case: Xp
    case spot::ltl::unop::X:
        // if we are at the terminal event, the next event is also a terminal
        // event. Since we are traversing a finite tree, this will terminate.
        if (is_terminal(trace_pt)) {
            return check(p, trace_pt, trace_ids);
        }
        return check_on_kids(p, get_next_event(trace_pt), trace_ids);

        // Finally case: Fp
    case spot::ltl::unop::F:
        // base case: if we're at END_VAR, return false to not effect ||
        if (is_terminal(trace_pt)) {
            return create_int_bool_map(trace_ids, false);
        } else {
            // check whether p is true
            map<int, bool> p_holds_here = check(p, trace_pt, trace_ids);
            // for those where p is true, add to return map and remove
            // from trace_ids to prevent recursion
            add_satisfying_values(p_holds_here, true, return_map, trace_ids);
            // add the values that were not truncated
            if (trace_ids.size() != 0) {
                map<int, bool> Fp_holds_on_rest = check_on_kids(node,
                        get_next_event(trace_pt), trace_ids);
                return_map.insert(Fp_holds_on_rest.begin(),
                        Fp_holds_on_rest.end());
            }
            return return_map;
        }

        // Globally case: Gp
    case spot::ltl::unop::G:
        // base case: if we're at END_VAR, return true to not effect &&
        if (is_terminal(trace_pt)) {
            return create_int_bool_map(trace_ids, true);
        } else {
            // check whether p is true
            map<int, bool> p_holds_here = check(p, trace_pt, trace_ids);
            // for those where p is false, add to return map and remove
            // from trace_ids to prevent recursion
            add_satisfying_values(p_holds_here, false, return_map, trace_ids);
            // add the values that were not truncated
            if (trace_ids.size() != 0) {
                map<int, bool> Fp_holds_on_rest = check_on_kids(node,
                        get_next_event(trace_pt), trace_ids);
                return_map.insert(Fp_holds_on_rest.begin(),
                        Fp_holds_on_rest.end());
            }
            return return_map;
        }

        // Not case: !p
    case spot::ltl::unop::Not:
        return not_map(check(p, trace_pt, trace_ids));

        // Other operators are not LTL, don't support them
    default:
        std::cerr << "Unsupported unary operator. Returning false. \n";
        return map<int, bool>();

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
map<int, bool> linear_trace_checker::check(const spot::ltl::multop* node,
        const trace_node trace_pt, set<int> trace_ids) {
    // is this an and or an or
    spot::ltl::multop::type opkind = node->op();
    // the return map;
    map<int, bool> return_map;

    switch (opkind) {

    case spot::ltl::multop::Or: {
        int numkids = node->size();
        // for each of the multop's children, we check if it is true: if it is,
        // we short circuit and return true. If we have not returned by the
        // end of the loop, then none of the children were true and we return
        // false.
        for (int i = 0; i < numkids; i++) {
            map<int, bool> kid_valids = check(node->nth(i), trace_pt,
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
    case spot::ltl::multop::And: {
        int numkids = node->size();
        // for each of the multop's children, we check if it is false: if it is,
        // we short circuit and return false. If we have not returned by the
        // end of the loop, then none of the children were false and we return
        // true.
        for (int i = 0; i < numkids; i++) {
            map<int, bool> kid_valids = check(node->nth(i), trace_pt,
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
    default:

        std::cerr << "Unsupported multiple operator. Returning false. \n";
        return map<int, bool>();

    }

}

/**
 * Gets the next node if this is a linear trace node.
 * @param current_node
 * @return
 */
map<set<int>, linear_trace_checker::trace_node> linear_trace_checker::get_next_event(
        const trace_node current_node) {
    trace_node return_node;
    get<const string_event*>(return_node) = get<const string_event*>(
            current_node) + 1;
    map<set<int>, linear_trace_checker::trace_node> return_map;
    return_map.emplace(get_trace_ids(current_node), return_node);
    return return_map;
}

/**
 * Returns true if the current node is terminal
 * @param current_node current position in trace
 * @return is this a terminal event
 */
bool linear_trace_checker::is_terminal(const trace_node current_node) {
    return get<const string_event*>(current_node)->is_terminal();
}

/**
 * Returns name of the current event
 * @param current_node current position in trace
 * @return is this a terminal event
 */
string linear_trace_checker::event_name(const trace_node current_node) {
    return get<const string_event*>(current_node)->get_name();
}

/**
 * Returns the trace_ids this event corresponds to
 * @param current_node
 * @return
 */
set<int> linear_trace_checker::get_trace_ids(const trace_node current_node) {
    set<int> return_set;
    return_set.insert(0);
    return return_set;

}

/**
 * Creates an int-bool map with all the same bool value and keys given by ids
 * @param ids keys of map
 * @param valid value of map
 * @return
 */
map<int, bool> linear_trace_checker::create_int_bool_map(set<int> ids,
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
map<int, bool> linear_trace_checker::not_map(map<int, bool> map) {
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
void linear_trace_checker::add_satisfying_values(map<int, bool>& returned_vals,
        bool to_satisfy, map<int, bool>& map_to_return, set<int>& to_check) {
    for (map<int, bool>::iterator it = returned_vals.begin();
            it != returned_vals.end(); it++) {
        if (it->second == to_satisfy) {
            map_to_return.insert(*it);
            to_check.erase(it->first);
        }

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
                    bool valid_on_trace = checker.check_on_trace(instantiated_prop_type, current_trace);
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
