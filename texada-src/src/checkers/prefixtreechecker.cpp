/*
 * prefixtreechecker.cpp
 *
 *  Created on: Jul 18, 2014
 *      Author: clemieux
 */

#include "prefixtreechecker.h"
#include "../instantiation-tools/apsubbingcloner.h"
#include "../instantiation-tools/subformulaapcollector.h"
#include "../instantiation-tools/texadatospotmapping.h"

namespace texada {

prefix_tree_checker::prefix_tree_checker() {
    use_invariant_semantics = false;
    translations = nullptr;
}

prefix_tree_checker::prefix_tree_checker(bool use_inv_s, shared_ptr<map<string,string>> ptr){
    use_invariant_semantics = use_inv_s;
    translations = ptr;
}

prefix_tree_checker::~prefix_tree_checker() {
    // TODO Auto-generated destructor stub
}

/**
 * Add the bindings map. Only need to be changed at different formulae,
 * not at different instantitaions.
 * @param bindings_map
 */
void prefix_tree_checker::add_relevant_bindings(
        map<const ltl::formula*, set<string>> * bindings_map) {
    relevant_bindings_map = bindings_map;
}

/**
 * Check on trace for memoization version.
 * @param form_node formula to check
 * @param trace_node beginning of trace to check on.
 * @param instantiations_ instantiation function for the formula
 * @return
 */
statistic prefix_tree_checker::check_on_trace(const ltl::formula* form_node,
        shared_ptr<prefix_tree_node> trace_node,
        map<string, string> instantiations_) {
    use_memo = true;
    instantiations = instantiations_;
    return check_on_trace(form_node, trace_node);
}

/**
 * Top-level check function that consolidates branch results.
 * @param node formula to check
 * @param trace_pt beginning of trace/prefix tree
 * @return
 */
statistic prefix_tree_checker::check_on_trace(const ltl::formula* node,
        const trace_node trace_pt) {
    set<int> set = trace_pt->get_trace_ids();
    map<int, statistic> branch_results = this->check(node, trace_pt, set);
    for (map<int, statistic>::iterator result_it = branch_results.begin();
            result_it != branch_results.end(); result_it++) {
        if (!(result_it->second).is_satisfied) {
            return statistic(false, 0, 1);
        }
    }
    return statistic(true, 1, 1);

}

/**
 * Explores all specified branches with node.
 * @param node
 * @param trace_pts the children of the previous branch point
 * @param trace_ids the branches to check
 * @return map of evaluation values.
 */
map<int, statistic> prefix_tree_checker::check_on_kids(
        const ltl::formula* node, map<set<int>, trace_node> kids,
        set<int> trace_ids) {
    // return map
    map<int, statistic> return_map;
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
            map<int, statistic> result_on_kid = this->check(node, kids_it->second,
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
map<int, statistic> prefix_tree_checker::false_check(std::set<int> trace_ids) {
    return create_int_bool_map(trace_ids, statistic(false, 0, 1));
}
/**
 * Return a true map
 * @param trace_ids
 * @return
 */
map<int, statistic> prefix_tree_checker::true_check(std::set<int> trace_ids) {
    return create_int_bool_map(trace_ids, statistic(true, 1, 1));
}

/**
 * Checking a single event on a trace means we check it on the first element of the trace
 * @param node: the atomic proposition to check
 * @param trace_pt: pointer to the start of the trace
 * @param trace_ids: trace ids we're checking on
 * @return whether node holds on trace
 */
map<int, statistic> prefix_tree_checker::ap_check(const ltl::atomic_prop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    if (use_memo) {
        if (instantiations.find(node->name()) != instantiations.end()) {
            std::string prop = (instantiations.find(node->name()))->second;
            if (trace_pt->is_satisfied(prop)) {
                return create_int_bool_map(trace_ids, statistic(true, 1, 1));
            } else {
#ifdef SMT_SUPPORT
                if (use_invariant_semantics){
                    if (ap_holds(trace_pt->get_event(), node->name(),translations)){
                        return create_int_bool_map(trace_ids, statistic(true, 1, 1));
                    } else return create_int_bool_map(trace_ids, statistic(false, 0, 1));
                } else
#endif
                    return create_int_bool_map(trace_ids, statistic(false, 0, 1));
            }
        } else {
            std::cerr << "Did not find mapping for " << node->name() <<". \n";
            return create_int_bool_map(trace_ids, statistic(false, 0, 1));
        }
    } else {
        // evaluate whether the AP holds
        if (trace_pt->is_satisfied(node->name())) {
            return create_int_bool_map(trace_ids, statistic(true, 1, 1));
        } else {
#ifdef SMT_SUPPORT
            if (use_invariant_semantics){
                if (ap_holds(trace_pt->get_event(), node->name(),translations)){
                    return create_int_bool_map(trace_ids, statistic(true, 1, 1));
                } else return create_int_bool_map(trace_ids, statistic(false, 0, 1));
            }  else
#endif
                return create_int_bool_map(trace_ids, statistic(false, 0, 1));
        }
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
map<int, statistic> prefix_tree_checker::xor_check(const ltl::binop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    const ltl::formula * p = node->first();
    const ltl::formula * q = node->second();
    map<int, statistic> return_map;
    //retrieve memoized values.
    if (use_memo) {
        //get memoized values
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        }
        // else add those that were memoized to return_map, and remove
        // them from trace_ids, so we only evaluate the missing ones.
        else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }
    // xor requires checking both p and q, no shortcuts here
    map<int, statistic> qholds = this->check(q, trace_pt, trace_ids);
    map<int, statistic> pholds = this->check(p, trace_pt, trace_ids);
    for (map<int, statistic>::iterator pholds_it = pholds.begin();
            pholds_it != pholds.end(); pholds_it++) {
        statistic qholds_on_trace = qholds.find(pholds_it->first)->second;
        if ((pholds_it->second).is_satisfied != qholds_on_trace.is_satisfied) {
            pholds_it->second = statistic(true, 1, 1);
        } else {
            pholds_it->second = statistic(false, 0, 1);
        }

    }
    if (use_memo) {
        add_to_memo_map(node, trace_pt, pholds);
        pholds.insert(return_map.begin(), return_map.end());
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
map<int, statistic> prefix_tree_checker::equiv_check(const ltl::binop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    const ltl::formula * p = node->first();
    const ltl::formula * q = node->second();
    map<int, statistic> return_map;
    //retrieve memoized values.
    if (use_memo) {
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        }
        // else add those that were memoized to return_map, and remove
        // them from trace_ids, so we only evaluate the missing ones.
        else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }
    // <-> requires checking both p and q, no shortcuts here
    map<int, statistic> qholds = this->check(q, trace_pt, trace_ids);
    map<int, statistic> pholds = this->check(p, trace_pt, trace_ids);
    for (map<int, statistic>::iterator pholds_it = pholds.begin();
            pholds_it != pholds.end(); pholds_it++) {
        statistic qholds_on_trace = qholds.find(pholds_it->first)->second;
        if ((pholds_it->second).is_satisfied == qholds_on_trace.is_satisfied) {
            pholds_it->second = statistic(true, 1, 1);
        } else {
            pholds_it->second = statistic(false, 0, 1);
        }
    }
    if (use_memo) {
        add_to_memo_map(node, trace_pt, pholds);
        pholds.insert(return_map.begin(), return_map.end());
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
map<int, statistic> prefix_tree_checker::implies_check(const ltl::binop* node,
        trace_node trace_pt, std::set<int> trace_ids) {

    const ltl::formula * p = node->first();
    const ltl::formula * q = node->second();
    map<int, statistic> return_map;
    //retrieve memoized values.
    if (use_memo) {
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        }
        // else add those that were memoized to return_map, and remove
        // them from trace_ids, so we only evaluate the missing ones.
        else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }

    map<int, statistic> pholds = this->check(p, trace_pt, trace_ids);
    // where p is false we don't need to recurse. Add those
    // pairs and remove trace_ids where !p. By doing the not map
    // we add where !p holds as true, as it should be.
    pholds = not_map(pholds);
    add_satisfying_values(pholds, true, return_map, trace_ids);
    // if some traces return p, then the value of p -> q is
    // the value of q on this point.
    if (trace_ids.size() != 0) {
        map<int, statistic> qholds = this->check(q, trace_pt, trace_ids);
        return_map.insert(qholds.begin(), qholds.end());
    }

    if (use_memo) {
        add_to_memo_map(node, trace_pt, return_map);
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
map<int, statistic> prefix_tree_checker::until_check(const ltl::binop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    const ltl::formula * p = node->first();
    const ltl::formula * q = node->second();
    map<int, statistic> return_map;
    //retrieve memoized values.
    if (use_memo) {
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        }
        // else add those that were memoized to return_map, and remove
        // them from trace_ids, so we only evaluate the missing ones.
        else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }
    //if we get here, we did not see q: thus, false
    if (trace_pt->is_terminal()) {
        return create_int_bool_map(trace_ids, statistic(false, 0, 1));
    }
    // if the q holds here, we have not yet seen q or !p, (these
    //  cause return) so true values are true overall. retain
    // the true values and remove the trace ids with true values
    // from trace_ids
    map<int, statistic> q_holds = this->check(q, trace_pt, trace_ids);
    add_satisfying_values(q_holds, true, return_map, trace_ids);
    // if q held on all traces, return
    if (trace_ids.size() == 0) {
        if (use_memo) {
            add_to_memo_map(node, trace_pt, return_map);
        }
        return return_map;
    }

    // we know q does not hold from above, so if p does not hold,
    // we have !p before q, which violates p U q.
    map<int, statistic> p_holds = this->check(p, trace_pt, trace_ids);
    // where !p, !(p U q), so insert as false:
    add_satisfying_values(p_holds, false, return_map, trace_ids);
    // if we've determined values on all traces, return
    if (trace_ids.size() == 0) {
        if (use_memo) {
            add_to_memo_map(node, trace_pt, return_map);
        }
        return return_map;
    }

    // if !q and p holds, check on the next suffix trace
    map<int, statistic> pUq_on_kids = check_on_kids(node, trace_pt->get_children(),
            trace_ids);
    // those values are the value of pUq on here
    return_map.insert(pUq_on_kids.begin(), pUq_on_kids.end());
    if (use_memo) {
        add_to_memo_map(node, trace_pt, return_map);
    }
    return return_map;

}

/**
 * p R q: q must be true up to and including the first occurrence of p
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, statistic> prefix_tree_checker::release_check(const ltl::binop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    const ltl::formula * p = node->first();
    const ltl::formula * q = node->second();
    map<int, statistic> return_map;
    //retrieve memoized values.
    if (use_memo) {
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        }
        // else add those that were memoized to return_map, and remove
        // them from trace_ids, so we only evaluate the missing ones.
        else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }
    //if we get here, q was always true, so true
    if (trace_pt->is_terminal()) {
        return create_int_bool_map(trace_ids, statistic(true, 1, 1));
    }
    // if the !q holds here, since we haven't yet seen p,
    // p R q is false (!q occurred before p)
    map<int, statistic> q_holds = this->check(q, trace_pt, trace_ids);
    add_satisfying_values(q_holds, false, return_map, trace_ids);
    // if !q held on all traces, return
    if (trace_ids.size() == 0) {
        if (use_memo) {
            add_to_memo_map(node, trace_pt, return_map);
        }
        return return_map;
    }

    // traces remaining in trace_ids where ones where q held, so
    // if p holds, p & q holds:
    map<int, statistic> p_holds = this->check(p, trace_pt, trace_ids);
    // where p, p & q, so p R q, so insert as true:
    add_satisfying_values(p_holds, true, return_map, trace_ids);
    // if we've determined values on all traces, return
    if (trace_ids.size() == 0) {
        if (use_memo) {
            add_to_memo_map(node, trace_pt, return_map);
        }
        return return_map;
    }

    // if !p and q, check on next suffix trace
    map<int, statistic> pRq_on_kids = check_on_kids(node, trace_pt->get_children(),
            trace_ids);
    // those values are the value of pRq on here
    return_map.insert(pRq_on_kids.begin(), pRq_on_kids.end());
    if (use_memo) {
        add_to_memo_map(node, trace_pt, return_map);
    }
    return return_map;
}

/**
 * p W q: p must be true until the first occurrence of q,
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, statistic> prefix_tree_checker::weakuntil_check(
        const ltl::binop* node, trace_node trace_pt,
        std::set<int> trace_ids) {
    const ltl::formula * p = node->first();
    const ltl::formula * q = node->second();
    map<int, statistic> return_map;
    //retrieve memoized values.
    if (use_memo) {
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        }
        // else add those that were memoized to return_map, and remove
        // them from trace_ids, so we only evaluate the missing ones.
        else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }
    //if we get here, we did not see q: thus, false
    if (trace_pt->is_terminal()) {
        if (use_memo) {
            add_to_memo_map(node, trace_pt, return_map);
        }
        return create_int_bool_map(trace_ids, statistic(true, 1, 1));
    }
    // if the q holds here, we have not yet seen q or !p, (these
    //  cause return) so true values are true overall. retain
    // the true values and remove the trace ids with true values
    // from trace_ids
    map<int, statistic> q_holds = this->check(q, trace_pt, trace_ids);
    add_satisfying_values(q_holds, true, return_map, trace_ids);
    // if q held on all traces, return
    if (trace_ids.size() == 0) {
        if (use_memo) {
            add_to_memo_map(node, trace_pt, return_map);
        }
        return return_map;
    }

    // we know q does not hold from above, so if p does not hold,
    // we have !p before q, which violates p W q.
    map<int, statistic> p_holds = this->check(p, trace_pt, trace_ids);
    // where !p, !(p W q), so insert as false:
    add_satisfying_values(p_holds, false, return_map, trace_ids);
    // if we've determined values on all traces, return
    if (trace_ids.size() == 0) {
        if (use_memo) {
            add_to_memo_map(node, trace_pt, return_map);
        }
        return return_map;
    }

    // if !q and p holds, check on the next suffix trace
    map<int, statistic> pWq_on_kids = check_on_kids(node, trace_pt->get_children(),
            trace_ids);
    // those values are the value of pWq on here
    return_map.insert(pWq_on_kids.begin(), pWq_on_kids.end());
    if (use_memo) {
        add_to_memo_map(node, trace_pt, return_map);
    }
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
map<int, statistic> prefix_tree_checker::strongrelease_check(
        const ltl::binop* node, trace_node trace_pt,
        std::set<int> trace_ids) {
    const ltl::formula * p = node->first();
    const ltl::formula * q = node->second();
    map<int, statistic> return_map;
    //retrieve memoized values.
    if (use_memo) {
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        }
        // else add those that were memoized to return_map, and remove
        // them from trace_ids, so we only evaluate the missing ones.
        else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }
    //if we get here, q was always true, so false
    if (trace_pt->is_terminal()) {
        return create_int_bool_map(trace_ids, statistic(false, 0, 1));
    }
    // if the !q holds here, since we haven't yet seen p,
    // p R q is false (!q occurred before p)
    map<int, statistic> q_holds = this->check(q, trace_pt, trace_ids);
    add_satisfying_values(q_holds, false, return_map, trace_ids);
    // if !q held on all traces, return
    if (trace_ids.size() == 0) {
        if (use_memo) {
            add_to_memo_map(node, trace_pt, return_map);
        }
        return return_map;
    }

    // traces remaining in trace_ids where ones where q held, so
    // if p holds, p & q holds:
    map<int, statistic> p_holds = this->check(p, trace_pt, trace_ids);
    // where p, p & q, so p M q, so insert as true:
    add_satisfying_values(p_holds, true, return_map, trace_ids);
    // if we've determined values on all traces, return
    if (trace_ids.size() == 0) {
        if (use_memo) {
            add_to_memo_map(node, trace_pt, return_map);
        }
        return return_map;
    }

    // if !p and q, check on next suffix trace
    map<int, statistic> pMq_on_kids = check_on_kids(node, trace_pt->get_children(),
            trace_ids);
    // those values are the value of pRq on here
    return_map.insert(pMq_on_kids.begin(), pMq_on_kids.end());
    if (use_memo) {
        add_to_memo_map(node, trace_pt, return_map);
    }
    return return_map;
}

/**
 * Globally case: given Gp, p must hold on every point of the subsequent trace
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, statistic> prefix_tree_checker::globally_check(const ltl::unop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    //renaming the child p
    const ltl::formula * p = node->child();
    // create return map:
    map<int, statistic> return_map;
    //retrieve memoized values.
    if (use_memo) {
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        } else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }
    // base case: if we're at terminal, return true to not effect and behaviour
    if (trace_pt->is_terminal()) {
        return create_int_bool_map(trace_ids, statistic(true, 1, 1));
    }
    // else add those that were memoized to return_map, and remove
    // them from trace_ids, so we only evaluate the missing ones.
    else {
        // check whether p is true
        map<int, statistic> p_holds_here = this->check(p, trace_pt, trace_ids);
        // for those where p is false, add to return map and remove
        // from trace_ids to prevent recursion
        add_satisfying_values(p_holds_here, false, return_map, trace_ids);
        // add the values that were not truncated
        if (trace_ids.size() != 0) {
            map<int, statistic> Fp_holds_on_rest = check_on_kids(node,
                    trace_pt->get_children(), trace_ids);
            return_map.insert(Fp_holds_on_rest.begin(), Fp_holds_on_rest.end());
        }
        if (use_memo) {
            add_to_memo_map(node, trace_pt, return_map);
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
map<int, statistic> prefix_tree_checker::finally_check(const ltl::unop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    //renaming the child p
    const ltl::formula * p = node->child();
    // create return map:
    map<int, statistic> return_map;
    //retrieve memoized values.
    if (use_memo) {
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        }
        // else add those that were memoized to return_map, and remove
        // them from trace_ids, so we only evaluate the missing ones.
        else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }
    // base case: if we're at terminal, return false to not effect or behavior
    if (trace_pt->is_terminal()) {
        return create_int_bool_map(trace_ids, statistic(false, 0, 1));
    } else {
        // check whether p is true
        map<int, statistic> p_holds_here = this->check(p, trace_pt, trace_ids);
        // for those where p is true, add to return map and remove
        // from trace_ids to prevent recursion
        add_satisfying_values(p_holds_here, true, return_map, trace_ids);
        // add the values that were not truncated
        if (trace_ids.size() != 0) {
            map<int, statistic> Fp_holds_on_rest = check_on_kids(node,
                    trace_pt->get_children(), trace_ids);
            return_map.insert(Fp_holds_on_rest.begin(), Fp_holds_on_rest.end());
        }
        if (use_memo) {
            add_to_memo_map(node, trace_pt, return_map);
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
map<int, statistic> prefix_tree_checker::next_check(const ltl::unop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    //renaming the child p
    const ltl::formula * p = node->child();

    // if we are at the terminal event, the next event is also a terminal
    // event. Since we are traversing a finite tree, this will terminate.
    if (trace_pt->is_terminal()) {
        return this->check(p, trace_pt, trace_ids);
    }
    map<int, statistic> return_map;
    //retrieve memoized values.
    if (use_memo) {
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        }
        // else add those that were memoized to return_map, and remove
        // them from trace_ids, so we only evaluate the missing ones.
        else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }

    map<int, statistic> kids_result = check_on_kids(p, trace_pt->get_children(),
            trace_ids);
    if (use_memo) {
        add_to_memo_map(node, trace_pt, kids_result);
        return_map.insert(kids_result.begin(), kids_result.end());
    } else {
        return_map = kids_result;
    }
    return return_map;

}

/**
 * Next case: given !p, p must not hold
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, statistic> prefix_tree_checker::not_check(const ltl::unop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    //renaming the child p
    const ltl::formula * p = node->child();
    map<int, statistic> return_map;
    //retrieve memoized values.
    if (use_memo) {
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        }
        // else add those that were memoized to return_map, and remove
        // them from trace_ids, so we only evaluate the missing ones.
        else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }
    map<int, statistic> kids_result = not_map(this->check(p, trace_pt, trace_ids));
    if (use_memo) {
        add_to_memo_map(node, trace_pt, kids_result);
        return_map.insert(kids_result.begin(), kids_result.end());
    } else {
        return_map = kids_result;
    }

    return return_map;

}

/**
 * Or: true if one of or's children is true
 * @param node formula to check
 * @param trace_pt current point in tree
 * @param trace_ids which branches to pursue
 * @return map of trace_id -> node holds on trace starting from trace_pt
 */
map<int, statistic> prefix_tree_checker::or_check(const ltl::multop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    // the return map;
    map<int, statistic> return_map;
    if (use_memo) {
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        }
        // else add those that were memoized to return_map, and remove
        // them from trace_ids, so we only evaluate the missing ones.
        else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }
    int numkids = node->size();
    // for each of the or's children, we check if it is true: if it is,
    // we short circuit and return true. If we have not returned by the
    // end of the loop, then none of the children were true and we return
    // false.
    for (int i = 0; i < numkids; i++) {
        map<int, statistic> kid_valids = this->check(node->nth(i), trace_pt,
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
        return_map.emplace(*it, statistic(false, 0, 1));

    }
    if (use_memo) {
        add_to_memo_map(node, trace_pt, return_map);
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
map<int, statistic> prefix_tree_checker::and_check(const ltl::multop* node,
        trace_node trace_pt, std::set<int> trace_ids) {
    // the return map;
    map<int, statistic> return_map;
    //retrieve memoized values.
    if (use_memo) {
        map<int, statistic> memoed_values = retrieve_memo(node, trace_pt, trace_ids);
        // if all the trace evals are memoized, return
        if (memoed_values.size() == trace_ids.size()) {
            return memoed_values;
        }
        // else add those that were memoized to return_map, and remove
        // them from trace_ids, so we only evaluate the missing ones.
        else {
            for (std::map<int, statistic>::iterator memoed_values_it =
                    memoed_values.begin();
                    memoed_values_it != memoed_values.end();
                    memoed_values_it++) {
                return_map.insert(*memoed_values_it);
                trace_ids.erase(memoed_values_it->first);
            }
        }
    }

    int numkids = node->size();
    // for each of the and's children, we check if it is false: if it is,
    // we short circuit and return false. If we have not returned by the
    // end of the loop, then none of the children were false and we return
    // true.
    for (int i = 0; i < numkids; i++) {
        map<int, statistic> kid_valids = this->check(node->nth(i), trace_pt,
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
        return_map.emplace(*it, statistic(true, 1, 1));

    }
    if (use_memo) {
        add_to_memo_map(node, trace_pt, return_map);
    }
    return return_map;

}

/**
 * Creates an int-bool map with all the same bool value and keys given by ids
 * @param ids keys of map
 * @param valid value of map
 * @return
 */
map<int, statistic> prefix_tree_checker::create_int_bool_map(set<int> ids,
        statistic valid) {
    map<int, statistic> return_map;
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
map<int, statistic> prefix_tree_checker::not_map(map<int, statistic> map) {
    for (std::map<int, statistic>::iterator it = map.begin(); it != map.end();
            it++) {
        it->second.is_satisfied = !it->second.is_satisfied;               // Dennis: this doesn't look right
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
void prefix_tree_checker::add_satisfying_values(map<int, statistic>& returned_vals,
        bool to_satisfy, map<int, statistic>& map_to_return, set<int>& to_check) {


    for (map<int, statistic>::iterator it = returned_vals.begin();
            it != returned_vals.end(); it++) {
        if (it->second.is_satisfied == to_satisfy) {
            map_to_return.insert(*it);
            to_check.erase(it->first);
        }
    }
}

/**
 * Adds the formula, point, relevant mappings and results to the memiozation map.
 * @param node
 * @param trace_pt
 * @param return_val
 */
void prefix_tree_checker::add_to_memo_map(const ltl::formula* node,
        trace_node trace_pt, map<int, statistic> return_val) {
    memo_key insert_key;
    insert_key.node = node;
    insert_key.trace_pt = trace_pt;
    set<string> relevant_vars = aps_of_form(node);
    map<string, string> relevant_map;
    for (set<string>::iterator var_it = relevant_vars.begin();
            var_it != relevant_vars.end(); var_it++) {
        std::map<string, string>::iterator current_mapping =
                instantiations.find(*var_it);

        if (current_mapping != instantiations.end()) {
            relevant_map.insert(*current_mapping);
            //TODO what if the =/= doesn't hold?
        }
    }
    insert_key.relevant_mappings = relevant_map;

    boost::unordered_map<memo_key, map<int, statistic>, hash_memo_key>::iterator find_this =
            memo_map.find(insert_key);
    if (find_this == memo_map.end()) {
        //todo: what can we infer about the value on unchecked traces???
        memo_map.emplace(insert_key, return_val);
    } else {
        for (map<int, statistic>::iterator it = return_val.begin();
                it != return_val.end(); it++) {
            find_this->second.insert(*it);
        }
    }
}

/**
 * Retrives memoized values for this evaluation, if they exist.
 * @param node
 * @param trace_pt
 * @param trace_ids
 * @return
 */
map<int, statistic> prefix_tree_checker::retrieve_memo(
        const ltl::formula* node, trace_node trace_pt,
        std::set<int> trace_ids) {
    // create the retrieval key
    memo_key retrieve_key;
    retrieve_key.node = node;
    retrieve_key.trace_pt = trace_pt;
    set<string> relevant_vars = aps_of_form(node);
    map<string, string> relevant_map;
    // get the correct mappings from the checker state.
    for (set<string>::iterator var_it = relevant_vars.begin();
            var_it != relevant_vars.end(); var_it++) {
        std::map<string, string>::iterator current_mapping =
                instantiations.find(*var_it);

        if (current_mapping != instantiations.end()) {
            relevant_map.insert(*current_mapping);
            //TODO what if the =/= doesn't hold?
        }
    }
    retrieve_key.relevant_mappings = relevant_map;

    // check if the values we want are in the memo map
    boost::unordered_map<memo_key, map<int, statistic>, hash_memo_key>::iterator find_this =
            memo_map.find(retrieve_key);
    if (find_this == memo_map.end()) {
        // if there's none, return blank.
        return map<int, statistic>();
    } else {
        // just return the results that are relevant to the trace ids we
        // are checking on.
        map<int, statistic> return_map;
        for (set<int>::iterator trace_it = trace_ids.begin();
                trace_it != trace_ids.end(); trace_it++) {
            // if we have the result for this trace, store it.
            if (find_this->second.find(*trace_it) != find_this->second.end()) {
                return_map.insert(*find_this->second.find(*trace_it));
            }

        }
        return return_map;
    }
}

/**
 * Find the atomic propositions belonging to node.
 * @param node
 * @return
 */
set<string> prefix_tree_checker::aps_of_form(const ltl::formula* node) {
    map<const ltl::formula*, set<string>>::iterator set_pair =
            relevant_bindings_map->find(node);
    if (set_pair == relevant_bindings_map->end()) {
        std::cerr
                << "Could not find the atomic props for this node in the map.";
        return set<string>();
        //TODO: exception
    }
    return set_pair->second;
}

/**
 * Clears the memoization map. By default, called after check on trace.
 */
void prefix_tree_checker::clear_memo_map(){
    memo_map.clear();
}

/**
 * Check all instantiations of the property type given on the traces
 * and return the valid ones
 * @param prop_type property type to check.
 * @param instantiator instantiator to produce next instantiation
 * @param traces all the traces to check on
 * @return
 */
vector<std::pair<map<string, string>, statistic>> valid_instants_on_traces(
        const ltl::formula * prop_type,
        instants_pool_creator * instantiator, shared_ptr<prefix_tree> traces, bool use_invariant_semantics,
        shared_ptr<map<string,string>> translations) {
    instantiator->reset_instantiations();
    // vector to return
    vector<std::pair<map<string, string>, statistic>> return_vec;
    prefix_tree_checker checker(use_invariant_semantics,translations);

    // create the ap collector for memoization
    subformula_ap_collector * collector = new subformula_ap_collector();
    prop_type->accept(*collector);
    checker.add_relevant_bindings(&collector->subform_ap_set);

    // get all the traces
    set<shared_ptr<prefix_tree_node>> iterable_traces =
            traces->get_start_nodes();
    while (true) {
        shared_ptr<map<string, string>> current_instantiation =
                instantiator->get_next_instantiation();
        if (current_instantiation == NULL) {
            break;
        }
        map<string,string> instantiation_to_pass = *current_instantiation;
        // easy work around the possibility of events instead of
        // variables: add to current_instantiation event->event
        vector<string> exclude_events = instantiator->get_events_to_exclude();
        if (exclude_events.size()!= 0){
            for (vector<string>::iterator events_it = exclude_events.begin(); events_it !=
                    exclude_events.end(); events_it++){
                instantiation_to_pass.emplace(*events_it,*events_it);
            }
        }

        // is the instantiation valid?
        statistic global_stat = statistic(true, 0, 0);
        for (set<shared_ptr<prefix_tree_node>>::iterator it =
                iterable_traces.begin(); it != iterable_traces.end(); it++) {
            global_stat = statistic(global_stat, checker.check_on_trace(prop_type, *it,
                    instantiation_to_pass));
            if (!global_stat.is_satisfied) {
                break;
            }
        }
        //instantiated_prop_type->destroy();
        if (global_stat.is_satisfied) {
            std::pair<map<string, string>, statistic> finding(*current_instantiation, global_stat);
            return_vec.push_back(finding);
        }
    }
    delete(collector);
    return return_vec;

}

} /* namespace texada */
