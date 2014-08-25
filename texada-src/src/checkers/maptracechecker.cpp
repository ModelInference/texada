/*
 * maptracechecker.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: clemieux
 */

#include "maptracechecker.h"
#include "../instantiation-tools/apsubbingcloner.h"
#include <ltlvisit/nenoform.hh>
#include <ltlvisit/tostring.hh>
#include <algorithm>
#include <deque>

namespace texada {

/**
 * Creates a map trace checker which can check any formula on the
 * trace map it's constructed on.
 * @param trace_map_
 */
map_trace_checker::map_trace_checker(
        const std::map<string_event, std::vector<long>>* trace_map_) :
        trace_map(trace_map_) {
    std::vector<long> end_vector = trace_map->at(texada::string_event());
    terminal_pos = end_vector[0];

}

/**
 * destructor
 */
map_trace_checker::~map_trace_checker() {
    first_occ_map.clear();
    last_occ_map.clear();
    trace_map = NULL;
}

/**
 * Checks whether a formula holds on the trace inputed to the map trace checker.
 * @param node formula to check
 * @return true if node holds on the trace, false otherwise
 */
bool map_trace_checker::check_on_trace(const spot::ltl::formula* node,
        interval intvl) {
    intvl.end = terminal_pos - 1;
    return this->check(node, intvl);

}
/**
 * Checks whether a atomic proposition holds on the given interval.
 * @param node formula to check
 * @param intvl interval to check on
 * @param trace_ids
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::ap_check(const spot::ltl::atomic_prop* node,
        interval intvl, std::set<int> trace_ids) {
    try {
        std::vector<long> to_search = trace_map->at(string_event(node->name()));
        if (std::binary_search(to_search.begin(), to_search.end(),
                intvl.start)) {
            return true;
        } else
            return false;
    } catch (std::out_of_range &e) {
        return false;
    }
}

/**
 * Checks until on the interval: given p U q, need q to occur and !p
 * not to occur before q.
 * @param node p U q to check
 * @param intvl interval to check on
 * @param trace_ids
 * @return
 */
bool map_trace_checker::until_check(const spot::ltl::binop* node,
        interval intvl, std::set<int> trace_ids) {
    const spot::ltl::formula* p = node->first();
    const spot::ltl::formula* q = node->second();
    // U deals with all the future, so extend interval
    intvl.end = terminal_pos - 1;
    // find q
    long first_occ_q = find_first_occurrence(q, intvl);
    // if q never occurs, until does not hold
    if (first_occ_q == -1) {
        return false;
    }

    // construct !p
    const spot::ltl::formula * not_p = spot::ltl::negative_normal_form(p, true);
    // check that !p never occurs
    long first_occ_not_p = find_first_occurrence(not_p, intvl);
    // clean up !p
    not_p->destroy();

    // make sure !p did not occur before q
    if (first_occ_not_p < first_occ_q) {
        return false;
    } else
        return true;

}

/**
 * Checks release on the interval: given p R q, q to hold up to and including
 * p's first occurrence
 * @param node p R q to check
 * @param intvl interval to check on
 * @param trace_ids
 * @return
 */
bool map_trace_checker::release_check(const spot::ltl::binop* node,
        interval intvl, std::set<int> trace_ids) {
    const spot::ltl::formula* p = node->first();
    const spot::ltl::formula* q = node->second();

    // need to make sure there's no violation of R after end of intvl
    intvl.end = terminal_pos - 1;

    // construct and find the first occurrence of !q
    const spot::ltl::formula * not_q = spot::ltl::negative_normal_form(q, true);
    long first_occ_not_q = find_first_occurrence(not_q, intvl);
    not_q->destroy();

    // find first occurrence of p
    long first_occ_p = find_first_occurrence(p, intvl);

    // if !q never occurs, p R q holds
    if (first_occ_not_q == -1) {
        return true;
    }
    // if p never occurs and !q occurred, false
    if (first_occ_p == -1) {
        return false;
    }
    // at the first p, q must still hold so the first !q
    // must be strictly after the first p
    else if (first_occ_not_q <= first_occ_p) {
        return false;
    } else
        return true;
}

/**
 * Checks weak until on the interval: given p U q, need !p
 * not to occur before q.
 * @param node p W q to check
 * @param intvl interval to check on
 * @param trace_ids
 * @return
 */
bool map_trace_checker::weakuntil_check(const spot::ltl::binop* node,
        interval intvl, std::set<int> trace_ids) {
    const spot::ltl::formula* p = node->first();
    const spot::ltl::formula* q = node->second();
    // need to make sure there's no violation of W after intvl.end
    intvl.end = terminal_pos - 1;
    // Find first q
    long first_occ_q = find_first_occurrence(q, intvl);

    // construct and find first !p
    const spot::ltl::formula * not_p = spot::ltl::negative_normal_form(p, true);
    long first_occ_not_p = find_first_occurrence(not_p, intvl);
    not_p->destroy();

    // if q never occurs, p W q holds only if !p never occurs
    if (first_occ_q == -1) {
        return (first_occ_not_p == -1);
    }
    // p W q holds if !p does not occur strictly before q
    else if (first_occ_not_p < first_occ_q) {
        return false;
    } else
        return true;
}

/**
 * Checks release on the interval: given p M q, q to hold up to and including
 * p's first occurrence, and p must occur
 * @param node p M q to check
 * @param intvl interval to check on
 * @param trace_ids
 * @return
 */
bool map_trace_checker::strongrelease_check(const spot::ltl::binop* node,
        interval intvl, std::set<int> trace_ids) {
    const spot::ltl::formula* p = node->first();
    const spot::ltl::formula* q = node->second();

    // need to make sure there's no violation of R after end of intvl
    intvl.end = terminal_pos - 1;

    // construct and find the first occurrence of !q
    const spot::ltl::formula * not_q = spot::ltl::negative_normal_form(q, true);
    long first_occ_not_q = find_first_occurrence(not_q, intvl);
    not_q->destroy();

    // find first occurrence of p
    long first_occ_p = find_first_occurrence(p, intvl);

    // if p never occurs, p M q does not hold
    if (first_occ_p == -1) {
        return false;
    }

    // if !q never occurs and p does, p M q holds
    if (first_occ_not_q == -1){
        return true;
    }

    // at the first p, q must still hold so the first !q
    // must be strictly after the first p
    else if (first_occ_not_q <= first_occ_p) {
        return false;
    } else
        return true;
}

/**
 * Checks whether Gp holds on the interval. Find first occurrence
 * of !p; if !p never occurs, Gp holds.
 * @param node Gp
 * @param intvl interval
 * @param trace_ids
 * @return
 */
bool map_trace_checker::globally_check(const spot::ltl::unop* node,
        interval intvl, std::set<int> trace_ids) {
    const spot::ltl::formula* p = node->child();
    // Globally operator holds on all future events, so
    // extend interval until terminal point
    intvl.end = terminal_pos - 1;
    //construct and find first !p
    const spot::ltl::formula * not_p = spot::ltl::negative_normal_form(p, true);
    long first_occ = find_first_occurrence(not_p, intvl);
    not_p->destroy();
    if (first_occ == -1)
        return true;
    else
        return false;
}

/**
 * Check whether Fp holds on the interval; find if p ever occurs
 * @param node Fp
 * @param intvl interval to check on
 * @param trace_ids
 * @return
 */
bool map_trace_checker::finally_check(const spot::ltl::unop* node,
        interval intvl, std::set<int> trace_ids) {
    const spot::ltl::formula* p = node->child();
    // Finally operator holds on all future events, so
    // extend interval until terminal point.
    intvl.end = terminal_pos - 1;
    long first_occ = find_first_occurrence(p, intvl);
    if (first_occ == -1)
        return false;
    else
        return true;
}

/**
 * Checks that Xp holds on the interval.
 * @param node Xp
 * @param intvl interval to check on
 * @param trace_ids
 * @return
 */
bool map_trace_checker::next_check(const spot::ltl::unop* node, interval intvl,
        std::set<int> trace_ids) {
    const spot::ltl::formula* p = node->child();
    // if the start and end of the interval are at the same place,
    // the next event is the terminal: check there
    if (intvl.start == intvl.end) {
        intvl.start = terminal_pos;
        intvl.end = terminal_pos;
        return this->check(p, intvl);
    }
    // move the start of the interval forward to check p on the next event.
    intvl.start++;
    return this->check(p, intvl);

}

/**
 * Finds the first occurrence of a formula in a given interval. Switch
 * method to helper functions
 * @param node formula to find
 * @param intvl interval to search in
 * @return first occurence of node in intvl, -1 if not found.
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::formula* node,
        interval intvl) {
    switch (node->kind()) {
    case spot::ltl::formula::Constant:
        return find_first_occurrence(
                static_cast<const spot::ltl::constant*>(node), intvl);
    case spot::ltl::formula::AtomicProp:
        return find_first_occurrence(
                static_cast<const spot::ltl::atomic_prop*>(node), intvl);
    case spot::ltl::formula::UnOp:
        return find_first_occurrence(static_cast<const spot::ltl::unop*>(node),
                intvl);
    case spot::ltl::formula::BinOp:
        return find_first_occurrence(static_cast<const spot::ltl::binop*>(node),
                intvl);
    case spot::ltl::formula::MultOp:
        return find_first_occurrence(
                static_cast<const spot::ltl::multop*>(node), intvl);
    case spot::ltl::formula::BUnOp:
        return find_first_occurrence(static_cast<const spot::ltl::bunop*>(node));
    case spot::ltl::formula::AutomatOp:
        return find_first_occurrence(
                static_cast<const spot::ltl::automatop*>(node));
    default:
        return -1;
    }
}

/**
 * Adds info the first_occ_map and return first_occ. Adds several
 * intervals which will have the same first occurrence
 * @param node formula we have first occ of
 * @param intvl interval on which we found node
 * @param first_occ the first occ of node on the interval intvl
 * @return first_occ
 */
long map_trace_checker::return_and_add_first(const spot::ltl::formula* node,
        interval intvl, long first_occ) {
    // we can add intervals with one-off start of interval if
    // the first occurrence is not the first of the interval
    if (first_occ != intvl.start) {
        formula_interval storer;
        storer.formula = node;
        storer.intvl.start = intvl.start;
        storer.intvl.end = intvl.end;
        //first-last
        first_occ_map.emplace(storer, first_occ);
        storer.intvl.start++;
        //first+1-last
        first_occ_map.emplace(storer, first_occ);
        storer.intvl.end++;
        //first+1-last+1;
        first_occ_map.emplace(storer, first_occ);
        storer.intvl.start--;
        //first-last+1;
        first_occ_map.emplace(storer, first_occ);
    } else {
        formula_interval storer;
        storer.formula = node;
        storer.intvl.start = intvl.start;
        storer.intvl.end = intvl.end;
        //first-last
        first_occ_map.emplace(storer, first_occ);
        storer.intvl.end++;
        //first-last+1;
        first_occ_map.emplace(storer, first_occ);

    }

    return first_occ;
}

/**
 * Adds info the last_occ_map and return last_occ.
 * @param node formula we have first occ of
 * @param intvl interval on which we found node
 * @param last_occ the last occ of node on the interval intvl
 * @return last_occ
 */
long map_trace_checker::return_and_add_last(const spot::ltl::formula* node,
        interval intvl, long last_occ) {
    formula_interval storer;
    storer.formula = node;
    storer.intvl.start = intvl.start;
    storer.intvl.end = intvl.end;
    //first-last
    last_occ_map.emplace(storer, last_occ);
    if (last_occ != intvl.end) {
        storer.intvl.end--;
        //first-last-1
        last_occ_map.emplace(storer, last_occ);

    }

    return last_occ;
}

/**
 * Finds the first occurrence of an atomic proposition in a given interval
 * @param node atomic prop to find
 * @param intvl interval to search in
 * @return first occurrence position, -1 if not found.
 */
long map_trace_checker::find_first_occurrence(
        const spot::ltl::atomic_prop* node, interval intvl) {
    // REQUIRES: to_search is sorted. this should be assured earlier on.
    formula_interval storer;
    storer.intvl.start = intvl.start;
    storer.intvl.end = intvl.end;
    storer.formula = node;
    std::unordered_map<formula_interval, long, formula_interval_hash>::iterator it =
            first_occ_map.find(storer);
    if (it != first_occ_map.end()) {
        return it->second;
    }
    try {
        std::vector<long> to_search = trace_map->at(string_event(node->name()));
        long left = 0;
        long right = to_search.size();
        long newpos;
        while (true) {
            newpos = (right + left) / 2;
            if (to_search[newpos] < intvl.start) {
                /**
                 * case where
                 * to_search:
                 * 8 33 41 54
                 *          ^newpos
                 * intvl: 62-88
                 * newpos is smaller than the start, but the last occurrence of the event; so event
                 * does not occur in intvl
                 */
                if (newpos + 1 >= to_search.size()) {
                    return return_and_add_first(node, intvl, -1);
                }
                if (to_search[newpos + 1] >= intvl.start) {
                    /**
                     * case where
                     * to_search:
                     * 8 33 41 54
                     *       ^newpos
                     * intvl: 47-67
                     * newpos is smaller than the start, but the next occurrence is within bounds
                     */
                    if (to_search[newpos + 1] <= intvl.end) {
                        return return_and_add_first(node, intvl,
                                to_search[newpos + 1]);
                    }
                    /**
                     * case where
                     * to_search:
                     * 8 33 41 54
                     *       ^newpos
                     * intvl: 47-51
                     * newpos is smaller than the start, but the next is not within bounds; event
                     * does not occur within intvl
                     */
                    else {
                        return return_and_add_first(node, intvl, -1);
                    }
                }

                /**
                 * case where
                 * to_search:
                 * 8 33 41 54 67 88
                 *       ^newpos
                 *intvl: 63-100
                 *
                 */
                left = newpos + 1;
            }
            /**
             * newpos is the start: this is the first occurrence
             */
            else if (to_search[newpos] == intvl.start) {
                return return_and_add_first(node, intvl, intvl.start);
            } else {
                if (newpos == 0) {
                    /**
                     * case where
                     * to_search:
                     * 14 33 41 54
                     *  ^newpos
                     *  intvl: 8-17
                     *  newpos is larger than the start, but smaller than the end
                     */
                    if (to_search[newpos] <= intvl.end) {
                        return return_and_add_first(node, intvl,
                                to_search[newpos]);
                    }
                    /**
                     * case where
                     * to_search:
                     * 14 33 41 54
                     *  ^newpos
                     *  intvl: 8-12
                     *  newpos is larger than the start and the end, so not in range
                     */
                    else {
                        return return_and_add_first(node, intvl, -1);
                    }
                }
                /**
                 * case where:
                 * to_search
                 * 8 33 47 54 67 88 91
                 * 			^newpos
                 * intvl: 7-69
                 * so the smaller interval on the right must contain the first occurrence
                 */
                right = newpos - 1;
            }
        }
    } catch (std::out_of_range &e) {
        // this means we didn't find the event in the trace map, so it never occurs;
        return return_and_add_first(node, intvl, -1);
    }
}

/**
 * Finds the first occurrence of a multop formula (and or or) in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return first occurrence of node in intvl
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::multop* node,
        interval intvl) {
    spot::ltl::multop::type opkind = node->op();
    switch (opkind) {

    // Or case: total_first_occ set to max. Go through all
    // children to find smallest first_occ
    case spot::ltl::multop::Or: {
        //set up for iteration
        int numkids = node->size();
        long total_first_occ = LONG_MAX;
        for (int i = 0; i < numkids; i++) {
            long first_occ = find_first_occurrence(node->nth(i), intvl);
            // guard against setting -1 occurrences as "first"
            if (first_occ != -1 && first_occ < total_first_occ) {
                total_first_occ = first_occ;
            }
        }
        // in this case no first occurrence was found
        if (total_first_occ == LONG_MAX)
            return -1;
        else
            return total_first_occ;
    }

        // in the and case the earliest occurrence of the and can only be
        // at the latest of the first occurrences of the children
    case spot::ltl::multop::And: {
        //set up iteration
        int numkids = node->size();
        long total_first_occ = -1;
        // find first of each child
        for (int i = 0; i < numkids; i++) {
            long first_occ = find_first_occurrence(node->nth(i), intvl);
            // if one of the children never occurs, neither does the and
            if (first_occ == -1){
                return -1;
            }
            if (first_occ > total_first_occ) {
                total_first_occ = first_occ;
            }
        }
        // check if that last first occurrence is the first occurrence
        // of the and by checking all the children there
        intvl.start = total_first_occ;
        for (int i = 0; i < numkids; i++) {
            // if any of them fail, this cannot be the first occurrence
            if (!this->check(node->nth(i), intvl)) {
                // find the first occurrence starting after the point
                // we just checked for first occurrence
                if (intvl.start != intvl.end) {
                    intvl.start++;
                    return find_first_occurrence(node, intvl);
                }
                // if we got to the end of the interval without finding it,
                // there is no first occurrence
                else
                    return -1;
            }
        }
        return total_first_occ;
    }

    default:
        std::cerr << "Unsupported Multop. Returning -1. \n";
        return -1;
    }
}

/**
 * Finds the first occurrence of a unop formula in a given interval
 * @param node unop formula to find
 * @param intvl interval to search in
 * @return first occurrence of node in intvl, -1 if not found.
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::unop* node,
        interval intvl) {
    spot::ltl::unop::type optype = node->op();

    switch (optype) {

    // Not case.
    case spot::ltl::unop::Not: {
        // Given !p, find first p
        long first_true = find_first_occurrence(node->child(), intvl);
        // if first occurrence of p is not the first event,
        // the first event is the first !p
        if (first_true == -1 || first_true > intvl.start) {
            return intvl.start;
        }
        // if it was a 1-event interval containing p, !p never occurs
        else if (intvl.start == intvl.end) {
            return -1;
        }
        // move interval forward to find first p later on
        else {
            intvl.start++;
            return find_first_occurrence(node, intvl);
        }
    }

        // Find the first occurrence of the child in the interval of nexts.
        // Return the corresponding event that comes before it.
    case spot::ltl::unop::X: {
        intvl.start++;
        intvl.end++;
        long first_occ = find_first_occurrence(node->child(), intvl);
        if (first_occ == -1)
            return -1;
        else {
            return first_occ - 1;
        }
    }

        // Globally: Given Gp, find the last occurrence of !p; first globally will
        // be after that.
    case spot::ltl::unop::G: {
        // creating extended interval to preserve intvl.end;
        // !p may occur after intvl.end and violate Gp
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_pos - 1;

        // create and find last occurrence of !p
        const spot::ltl::formula * neg_norm_child =
                spot::ltl::negative_normal_form(node->child(), true);
        long last_neg_occ = find_last_occurrence(neg_norm_child, temp);
        neg_norm_child->destroy();

        // if !p never occurs, Gp starts at the start
        if (last_neg_occ == -1)
            return intvl.start;
        // if last !p is after the end of intvl, Gp
        // never occurs in interval
        if (last_neg_occ >= intvl.end)
            return -1;
        // else the first Gp is after the last !p
        else
            return ++last_neg_occ;
    }

        // Finally: given Fp, find the first occurrence of p from the start of the
        // interval to the end of the trace. If it occurs at any time, then the first
        // Fp is at the front of the interval.
    case spot::ltl::unop::F: {
        intvl.end = terminal_pos - 1;
        long first_occ = find_first_occurrence(node->child(), intvl);
        if (first_occ == -1)
            return -1;
        else
            return intvl.start;
    }

    default:
        std::cerr << "Unsupported unary operation. Returning -1. \n";
        return -1;

    }

}

/**
 * Finds the first occurrence of a binop in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return first occurrence of node in intvl
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::binop* node,
        interval intvl) {
    spot::ltl::binop::type opkind = node->op();

    switch (opkind) {

    //XOr case: Take the first of each one, unless they're at the same place.
    // Return the smallest of the first. If they're at the same place, we'll
    // need to find the next firsts.
    case spot::ltl::binop::Xor: {
        // if we have p xor q, find first p and q
        long first_occ_first = find_first_occurrence(node->first(), intvl);
        long first_occ_second = find_first_occurrence(node->second(), intvl);
        // if p never occurs, first xor is first q
        // (if q also doesn't occur, xor does not, so -1 is returned)
        if (first_occ_first == -1)
            return first_occ_second;
        // if q never occurs, first xor is first p
        else if (first_occ_second == -1)
            return first_occ_first;
        // return earlier of p and q first occurrences
        else if (first_occ_first < first_occ_second)
            return first_occ_first;
        else if (first_occ_first > first_occ_second)
            return first_occ_second;
        // at this point we know p and q first occur at the same time.
        // if we have a one-element interval, xor doesn't occur
        else if (first_occ_second == intvl.end)
            return -1;
        // else find on interval starting after the pair
        else {
            intvl.start = first_occ_second + 1;
            return find_first_occurrence(node, intvl);
        }
    }

        // Find first of negation of first, and find first of validity of last.
        // return whichever is earlier.
    case spot::ltl::binop::Implies: {
        // Given p -> q, find first !p
        const spot::ltl::formula * neg_norm_first =
                spot::ltl::negative_normal_form(node->first(), true);
        long first_occ_neg_first = find_first_occurrence(neg_norm_first, intvl);
        neg_norm_first->destroy();
        // find first q
        long first_occ_second = find_first_occurrence(node->second(), intvl);

        // if !p never occurs, first -> is first q
        // (if q also doesn't occur, -> does not, so -1 is returned)
        if (first_occ_neg_first == -1)
            return first_occ_second;
        // if q never occurs, first -> is first !p
        else if (first_occ_second == -1)
            return first_occ_neg_first;
        // else return earliest one
        else if (first_occ_neg_first < first_occ_second)
            return first_occ_neg_first;
        else
            return first_occ_second;

    }

        //Equiv case: return first where neither occur, of first where both occur.
    case spot::ltl::binop::Equiv: {
        // given p<->q, find first p and q
        long first_occ_first = find_first_occurrence(node->first(), intvl);
        long first_occ_second = find_first_occurrence(node->second(), intvl);
        // if !p and !q at the start of the interval, p<->q holds there
        if (first_occ_first != intvl.start && first_occ_second != intvl.start) {
            return intvl.start;
        }
        // if p and q occur at the same spot, p <-> q holds
        else if (first_occ_first == first_occ_second) {
            return first_occ_first;
        }
        // if p <-> q does not hold on a one-element interval, it does not occur
        else if (intvl.start >= intvl.end) {
            return -1;
        }
        // go check on the next interval
        else {
            intvl.start++;
            return find_first_occurrence(node, intvl);
        }

    }

        // Until case: given p U q, find first occurrence of q and last occurrence
        // of !p before it
    case spot::ltl::binop::U: {
        // q might first occur after the end of intvl;
        // but also need to preserve intvl.end
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_pos - 1;

        // find first q in extended interval
        long first_occ_second = find_first_occurrence(node->second(), temp);
        // until needs q to occur; if it does not
        if (first_occ_second == -1)
            return -1;

        // find the last !p that occurs before the first q
        temp.end = first_occ_second - 1;
        const spot::ltl::formula * neg_norm_first =
                spot::ltl::negative_normal_form(node->first(), true);
        long last_occ_neg_first = find_last_occurrence(neg_norm_first, temp);
        neg_norm_first->destroy();

        // if !p doesn't occur before the first q, p U q holds
        // on the first element of the interval
        if (last_occ_neg_first == -1)
            return intvl.start;
        // if the last !p before is after the end of our original
        // interval, p U q hodls nowhere on that interval
        if (last_occ_neg_first >= intvl.end) {
            return -1;
        }
        // first p U q occurs after the last !p before q.
        else
            return ++last_occ_neg_first;
    }

        //Weak until case: identical to until except base case. Given p W q...
    case spot::ltl::binop::W: {
        // q might first occur after the end of intvl;
        // but also need to preserve intvl.end
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_pos - 1;

        // find first q
        long first_occ_second = find_first_occurrence(node->second(), temp);
        // set up for last !p before q
        long last_occ_neg_first;
        // if q never occurs, p W q can still hold in !p never occurs on
        // the entire interval
        if (first_occ_second == -1) {
            // create and find last occurrence of !p
            const spot::ltl::formula * neg_norm_first =
                    spot::ltl::negative_normal_form(node->first(), true);
            last_occ_neg_first = find_last_occurrence(neg_norm_first, temp);
            neg_norm_first->destroy();
            // if !p never occurs, p W q first occurs at the
            // start of the interval
            if (last_occ_neg_first == -1)
                return intvl.start;
            // if last !p is after the end of intvl,
            // p W q does not hold on intvl
            else if (last_occ_neg_first >= intvl.end)
                return -1;
            // if last !p occurs in the middle of intvl,
            // p W q first occurs after that !p.
            else
                return ++last_occ_neg_first;
        }
        // here q occurs.
        // change temp end to find last !p before q
        temp.end = first_occ_second - 1;
        // find last !p before q
        const spot::ltl::formula * neg_norm_first =
                spot::ltl::negative_normal_form(node->first(), true);
        last_occ_neg_first = find_last_occurrence(neg_norm_first, temp);
        neg_norm_first->destroy();
        // if !p never occurs, p W q first occurs at the start of the interval
        if (last_occ_neg_first == -1)
            return intvl.start;
        // if last !p is after the end of intvl, p W q does not hold on intvl
        else if (last_occ_neg_first >= intvl.end) {
            return -1;
        }
        // if last !p occurs in the middle of intvl,  p W q first
        // occurs after that !p.
        else
            return ++last_occ_neg_first;
    }

        // Release case:
    case spot::ltl::binop::R: {
        // given q R p; first q might occur after the end
        // of intvl, so create extended interval
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_pos - 1;

        // find first q on extended interval
        long first_occ_first = find_first_occurrence(node->first(), temp);
        long last_occ_neg_second;
        // if q never occurs, q R p can still hold in !p never occurs on
        // the entire interval
        if (first_occ_first == -1) {
            // create and first last !p on extended interval
            const spot::ltl::formula * neg_norm_second =
                    spot::ltl::negative_normal_form(node->second(), true);
            last_occ_neg_second = find_last_occurrence(neg_norm_second, temp);
            neg_norm_second->destroy();
            // if !p never occurs, q R p holds at the first point of the interval
            if (last_occ_neg_second == -1)
                return intvl.start;
            // last !p is after the end of intvl, q R p never holds on intvl
            else if (last_occ_neg_second >= intvl.end){
                return -1;}
            // last !p in middle of intvl, q R p starts after that.
            else
                return ++last_occ_neg_second;
        }
        // set temp to search for last !p before (inclusive) q
        temp.end = first_occ_first;
        // create and find last !p before (inclusive) first q
        const spot::ltl::formula * neg_norm_second =
                spot::ltl::negative_normal_form(node->second(), true);
        last_occ_neg_second = find_last_occurrence(neg_norm_second, temp);
        neg_norm_second->destroy();
        // if !p occurs when q does, it is not a case of release
        if (last_occ_neg_second == temp.end) {
             // if it's the end of the original intvl, we haven't found
            // q R p
            if (intvl.end <= temp.end)
                return -1;
            // else check to see if we find it at the next q
            intvl.start = ++temp.end;
            return find_first_occurrence(node, intvl);
        }
        // if !p never occurs, q R p first occurs
        // at the start of the interval

        else if (last_occ_neg_second == -1) {
            return intvl.start;
        }
        // last !p in middle of intvl, q R p starts after that
        else {
            return ++last_occ_neg_second;
        }
    }

        // Strong release: identical to release except base case:
    case spot::ltl::binop::M: {
        // given q M p first q might occur after the end
        // of intvl, so create extended interval
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_pos - 1;
        // find first q
        long first_occ_first = find_first_occurrence(node->first(), temp);
        // if q never occurs, neither does q M p.
        if (first_occ_first == -1){
             return -1;}
        // set temp to search for last !p before (inclusive) q
        temp.end = first_occ_first;
        // create and find last !p before (inclusive) q
        const spot::ltl::formula * neg_norm_second =
                spot::ltl::negative_normal_form(node->second(), true);
        long last_occ_neg_second = find_last_occurrence(neg_norm_second, temp);
         neg_norm_second->destroy();
        // if !p occurs when q does, q M p does not hold there
        if (last_occ_neg_second == temp.end) {
            // if that q was out of the interval, q M p
            // never holds on the interval
            if (intvl.end <= temp.end)
                return -1;
            // else we can check if q M p will hold for the next
            // q on the interval.
            intvl.start = ++temp.end;
            return find_first_occurrence(node, intvl);
        }
        // if !p never occurs, q M p first occurs
        // at the start of the interval
        if (last_occ_neg_second == -1) {
            return intvl.start;
        }
        // last !p in middle of intvl, q M p starts after that
        else
            return ++last_occ_neg_second;
    }

    default:
        std::cerr << "Unsupported binary operation. Returning -1. \n";
        return -1;
    }
}

/**
 * Finds the first occurrence of a constant in a given interval
 * @param node constant to find occurrence of
 * @param intvl interval to check in
 * @return first occurrence of node in intvl
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::constant *node,
        interval intvl) {
    spot::ltl::constant::type value = node->val();
    switch (value) {
    case spot::ltl::constant::True:
        return intvl.start;
    case spot::ltl::constant::False:
        return -1;
    case spot::ltl::constant::EmptyWord:
        std::cerr << "We came across the empty word. Returning -1. \n";
        return -1;
    default:
        return -1;
    }
}

/**
 * Finds the last occurrence of a formula in a given interval. Switch
 * method to helper functions.
 * @param node formula to find
 * @param intvl interval to search in
 * @return last occurence of node in intvl, -1 if not found.
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::formula* node,
        interval intvl) {
    switch (node->kind()) {
    case spot::ltl::formula::Constant:
        return find_last_occurrence(
                static_cast<const spot::ltl::constant*>(node), intvl);
    case spot::ltl::formula::AtomicProp:
        return find_last_occurrence(
                static_cast<const spot::ltl::atomic_prop*>(node), intvl);
    case spot::ltl::formula::UnOp:
        return find_last_occurrence(static_cast<const spot::ltl::unop*>(node),
                intvl);
    case spot::ltl::formula::BinOp:
        return find_last_occurrence(static_cast<const spot::ltl::binop*>(node),
                intvl);
    case spot::ltl::formula::MultOp:
        return find_last_occurrence(static_cast<const spot::ltl::multop*>(node),
                intvl);
    case spot::ltl::formula::BUnOp:
        return find_last_occurrence(static_cast<const spot::ltl::bunop*>(node));
    case spot::ltl::formula::AutomatOp:
        return find_last_occurrence(
                static_cast<const spot::ltl::automatop*>(node));
    default:
        return -1;
    }
}

/**
 * Finds the last occurrence of an atomic proposition in a given interval
 * @param node atomic prop to find
 * @param intvl interval to search in
 * @return last occurrence position, -1 if not found.
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::atomic_prop* node,
        interval intvl) {
    // REQUIRES: to_search is sorted. this should be assured earlier on.
    formula_interval storer;
    storer.intvl.start = intvl.start;
    storer.intvl.end = intvl.end;
    storer.formula = node;
    std::unordered_map<formula_interval, long, formula_interval_hash>::iterator it =
            last_occ_map.find(storer);
    if (it != last_occ_map.end()) {
        return it->second;
    }
    try {
        std::vector<long> to_search = trace_map->at(string_event(node->name()));

        long left = 0;
        long right = to_search.size() - 1;
        long newpos;
        while (true) {
            newpos = (right + left) / 2;
            if (to_search[newpos] > intvl.end) {

                // case where all is to the left
                if (newpos == 0) {
                    return return_and_add_last(node, intvl, -1);
                }
                //else if the previous is smaller than or equal to the end
                if (to_search[newpos - 1] <= intvl.end) {
                    // if it's not smaller than the start, return. Else we fail to find it
                    if (to_search[newpos - 1] >= intvl.start) {
                        return to_search[newpos - 1];
                    } else {
                        return return_and_add_last(node, intvl, -1);
                    }
                }
                // if it's bigger than intvl.end and the one before was also too big,
                // we need to look at the left side
                right = newpos - 1;
            }

            else if (to_search[newpos] == intvl.end) {
                return return_and_add_last(node, intvl, intvl.end);
            }
            // else to_search[newpos]<intvl.end
            else {
                // if it's the last element
                if (newpos >= to_search.size() - 1) {
                    // and it's larger than the start, then it's the last event
                    if (to_search[newpos] >= intvl.start) {
                        return return_and_add_last(node, intvl,
                                to_search[newpos]);
                    }
                    // else if it's smaller than the start, it's not in the interval
                    else {
                        return return_and_add_last(node, intvl, -1);
                    }
                }
                // else newpos is too small so we have to go to the right side
                left = newpos + 1;
            }
        }
    } catch (std::out_of_range &e) {
        // this means we didn't find the event in the trace map, so it never occurs;
        return return_and_add_last(node, intvl, -1);
    }
}
/**
 * Finds the last occurrence of a multop formula (and or or) in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return last occurrence of node in intvl
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::multop* node,
        interval intvl) {
    spot::ltl::multop::type opkind = node->op();
    switch (opkind) {

    // Or case: total_last_occ set to min. If the last occ of any of the events
    // is greater than total_last_occ, set total_last_occ to that.
    case spot::ltl::multop::Or: {
        int numkids = node->size();
        long total_last_occ = -1;
        for (int i = 0; i < numkids; i++) {
            long last_occ = find_last_occurrence(node->nth(i), intvl);
            if (last_occ > total_last_occ) {
                total_last_occ = last_occ;
            }
        }
        return total_last_occ;
    }

        // And case: Similar to finding first; take the smallest of the last occs
        // and check all other events there. If one doesn't check out, then
        // if there's still space before us in the original search interval, we find
        // the last there; if there isn't, we return -1.
    case spot::ltl::multop::And: {
        int numkids = node->size();
        long total_last_occ = LONG_MAX;
        // take the earliest of last occurrences of all children
        for (int i = 0; i < numkids; i++) {
            long last_occ = find_last_occurrence(node->nth(i), intvl);
            if (last_occ == -1)
                return -1;
            if (last_occ < total_last_occ) {
                total_last_occ = last_occ;
            }
        }
        // creating interval to check children on
        interval temp;
        temp.start = total_last_occ;
        temp.end = intvl.end;
        // check each child at the possible and location
        for (int i = 0; i < numkids; i++) {
            // if one fails
            if (!this->check(node->nth(i), temp)) {
                // if the interval is not one long, we can move
                // it to try and find the last in the previous part
                if (intvl.start < temp.start) {
                    intvl.end = total_last_occ - 1;
                    return find_last_occurrence(node, intvl);
                }
                // one of the children failed on the last option
                // in the interval, and does not occur.
                else
                    return -1;
            }
        }
        // if we get here, we passed through the loop
        // and found each child was valid on the point, so the
        // and holds
        return total_last_occ;
    }

    default:
        std::cerr << "Unsupported Multop. Returning -1. \n";
        return -1;
    }
}

/**
 * Finds the last occurrence of a unop formula in a given interval
 * @param node unop formula to find
 * @param intvl interval to search in
 * @return last occurrence of node in intvl, -1 if not found.
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::unop* node,
        interval intvl) {
    spot::ltl::unop::type optype = node->op();

    switch (optype) {

    // Not case:
    case spot::ltl::unop::Not: {
        // given !p, find first p in the interval
        long last_child = find_last_occurrence(node->child(), intvl);
        // if p is the last event
        if (last_child == intvl.end) {
            // and intvl is one long, p never occurs
            if (intvl.start == intvl.end)
                return -1;
            // else find last by pushing end of intvl back
            else {
                intvl.end--;
                return find_last_occurrence(node, intvl);
            }
        }
        // else the last p was not the last event, so the
        // last event is !p
        else
            return intvl.end;
    }

        // Next case:
    case spot::ltl::unop::X: {
        // shift interval so we can switch the question from
        // find Xp to find p
        intvl.start++;
        intvl.end++;
        // find last p
        long last_operand = find_last_occurrence(node->child(), intvl);
        // if p never occurs, neither does Xp
        if (last_operand == -1)
            return -1;
        // else it's one before the last p
        else
            return last_operand - 1;
    }

        // Globally case:
    case spot::ltl::unop::G: {
        // given Gp:
        // extend interval to check for last !p in the end
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_pos - 1;
        // create and find last !p
        const spot::ltl::formula * neg_norm_child =
                spot::ltl::negative_normal_form(node->child(), true);
        long last_neg_child = find_last_occurrence(neg_norm_child, temp);
        neg_norm_child->destroy();

        // is last !p is after the end, never has Gp on intvl
        if (last_neg_child >= intvl.end)
            return -1;
        else
            return intvl.end;
    }

        // Finally case:
    case spot::ltl::unop::F: {
        // Given Fp:
        //
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_pos - 1;
        // find last p
        long last_occ_child = find_last_occurrence(node->child(), intvl);
        // is the last occurrence is out of range, the last Fp
        // is at the end of intvl
        if (last_occ_child > intvl.end)
            return intvl.end;
        // else the last finally is where the last p is.
        else
            return last_occ_child;
    }

    default:
        std::cerr << "Unsupported unary operator. Returning -1. \n";
        return -1;

    }
}

/**
 * Finds the last occurrence of a constant in a given interval
 * @param node constant to find occurrence of
 * @param intvl interval to check in
 * @return last occurrence of node in intvl
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::constant* node,
        interval intvl) {
    spot::ltl::constant::type value = node->val();
    switch (value) {
    case spot::ltl::constant::True:
        return intvl.end;
    case spot::ltl::constant::False:
        return -1;
    case spot::ltl::constant::EmptyWord:
        std::cerr << "We came across the empty word. Returning -1. \n";
        return -1;
    default:
        return -1;
    }
}
/**
 * Finds the last occurrence of a binop in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return last occurrence of node in intvl
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::binop* node,
        interval intvl) {
    spot::ltl::binop::type opkind = node->op();

    switch (opkind) {

    // Xor case:
    case spot::ltl::binop::Xor: {
        // given p xor q, find last p and last q
        long last_first = find_last_occurrence(node->first(), intvl);
        long last_second = find_last_occurrence(node->second(), intvl);
        // if they're at the same spot
        if (last_second == last_first) {
            // and never occur, then there is never xor
            if (last_second == -1)
                return -1;
            // and at the first element, there is never xor
            // (nothing before the first)
            if (last_second == intvl.start)
                return -1;
            // and at some other point, check for the last
            // in the interval before
            else {
                intvl.end = last_second - 1;
                return find_last_occurrence(node, intvl);
            }
        }
        // if they occur at different points, return the later one
        if (last_second > last_first)
            return last_second;
        else
            return last_first;
    }

        // Return the last negation of the first or occurrence of the last.
        // If they are both equal to -1, then the last else will be entered
        // and -1 will be returned.
    case spot::ltl::binop::Implies: {
        // given p -> q
        // construct and find last !p
        const spot::ltl::formula * neg_norm_first =
                spot::ltl::negative_normal_form(node->first(), true);
        long last_neg_first = find_last_occurrence(neg_norm_first, intvl);
        neg_norm_first->destroy();
        //find last q
        long last_second = find_last_occurrence(node->second(), intvl);
        // return the later of the two (if neither occur, -1 is returned
        // in else case)
        if (last_neg_first > last_second)
            return last_neg_first;
        else
            return last_second;
    }

        // Equiv case: p <-> q
    case ::spot::ltl::binop::Equiv: {
        // find last p and last q
        long last_first = find_last_occurrence(node->first(), intvl);
        long last_second = find_last_occurrence(node->second(), intvl);
        // if they occur at the same spot, they either occur
        // at the last event or before that, so the last
        // event has !p & !q (satisfies equiv) of p & q (satisfies equiv)
        if (last_first == last_second)
            return intvl.end;
        // else if either of them is at the last event
        else if (last_first == intvl.end || last_second == intvl.end) {
            // it's a one-event interval, equiv doesn't occur
            if (intvl.end == intvl.start)
                return -1;
            // push back to find next lasts instead
            else {
                intvl.end--;
                return find_last_occurrence(node, intvl);
            }
        }
        //
        else
            return intvl.end;

    }

        // Until case: given p U q, if there is a q after the end
        // of intvl, the last element of interval might be p U q;
        // else the last q in the interval is the last p U q.
    case ::spot::ltl::binop::U: {

        if (intvl.end < terminal_pos - 1) {
            // create interval after interval
            interval temp;
            temp.start = intvl.end;
            temp.end = terminal_pos - 1;
            // find first q after the end of intvl
            long next_first_second = find_first_occurrence(node->second(),
                    temp);
            // if q never occurs there, then find last q
            // in the original intvl
            if (next_first_second == -1)
                return find_last_occurrence(node->second(), intvl);
            // if the last q is in the original intvl, return
            if (next_first_second == temp.start)
                return temp.start;
            // change temp to check never !p  before q
            temp.end = next_first_second - 1;
            // find last !p b/w last of orignal interal and next q after
            // interval end
            const spot::ltl::formula* neg_norm_first =
                    spot::ltl::negative_normal_form(node->first(), true);
            long last_first = find_last_occurrence(neg_norm_first, temp);
            neg_norm_first->destroy();
            // interval's end is only p U q if there is no !p between
            // it and the next q
            if (last_first == -1)
                return temp.start;
        }
        // if there is no q after the end of intvl, last p U q is last q.
        return find_last_occurrence(node->second(), intvl);
    }

        // Similar to until, given p W q:
    case ::spot::ltl::binop::W: {
        // create !p
        const spot::ltl::formula* neg_norm_first =
                spot::ltl::negative_normal_form(node->first(), true);

        if (intvl.end < terminal_pos - 1) {
            // create interval after the original interval
            // to check for q there
            interval temp;
            temp.start = intvl.end;
            temp.end = terminal_pos - 1;

            // find the first q in the "after" interval
            long next_first_second = find_first_occurrence(node->second(),
                    temp);
            // if q never occurs there
            if (next_first_second == -1) {
                // if never !p, the p W q holds on the last of
                // the interval
                if (find_last_occurrence(neg_norm_first, temp) == -1) {
                    neg_norm_first->destroy();
                    return intvl.end;
                }
            }
            // if the next q is right at the end of the interval, p W q holds there
            else if (next_first_second == temp.start)
                return temp.start;
            else {
                // check that there is never !p between end of interval
                // and the q after
                temp.end = next_first_second - 1;
                long last_first = find_last_occurrence(neg_norm_first, temp);
                // if there isn't, retun the end of the interval.
                if (last_first == -1) {
                    neg_norm_first->destroy();
                    return temp.start;
                }
            }
        }
        // find the last q on the original interval
        long last_second = find_last_occurrence(node->second(), intvl);
        // if q never occurs
        if (last_second == -1) {
            // make sure there's never a !p
            long last_neg_first = find_last_occurrence(neg_norm_first, intvl);
            neg_norm_first->destroy();
            // if the last !p is before the end, the end is still p W q
            if (last_neg_first != intvl.end)
                return intvl.end;
            // if it's at the end, then p W q never occurs.
            else
                return -1;
        }
        // if q ever occurs, return that postition as the last p W q
        else
            return last_second;
    }

        // Release case: q R p
        // Similar to weak until, but due to pickiness around having q & p at
        // the release case, recurses.
    case ::spot::ltl::binop::R: {
        // Given q R p:
        // create !p
        const spot::ltl::formula* neg_norm_second =
                spot::ltl::negative_normal_form(node->second(), true);
        // create the interval "after" intvl
        interval temp;
        temp.start = intvl.end;
        //usually we put temp end at terminal - 1 , but make sure not to make
        // temp.start > temp.end
        (intvl.end == terminal_pos) ?
                temp.end = terminal_pos : temp.end = terminal_pos - 1;
        //finding the first q
        long next_first_first = find_first_occurrence(node->first(), temp);
        // if there is no q in the "after" interval
        if (next_first_first == -1) {
            // if never !p on that interval we're okay
            if (find_last_occurrence(neg_norm_second, temp) == -1) {
                neg_norm_second->destroy();
                return intvl.end;
            }
        }
        // if q occurs in the "after" interval
        else {
            //set temp to check for !p before and including first occ q
            temp.end = next_first_first;
            long last_neg_second = find_last_occurrence(neg_norm_second, temp);
            neg_norm_second->destroy();

            // if !p never occurs we return end of the interval
            if (last_neg_second == -1)
                return intvl.end;
        }

        // else we check in the interval, finding last q
        long last_first = find_last_occurrence(node->first(), intvl);
        // if there's never a q, we know that later on there's a !p b/c
        // we didn't return in the (next_first_first == -1) clause,
        // so q R p does not hold
        if (last_first == -1) {
            return -1;
        }
        temp.start = last_first;
        // check to see if p holds at q; if it does, return that position
        if (this->check(node->second(), temp)) {
            return last_first;
        }
        // else if it doesn't hold and we don't have any other choices
        else if (intvl.start == last_first)
            return -1;
        // or we recurse to find other qs
        else {
            intvl.end = last_first - 1;
            return find_last_occurrence(node, intvl);
        }
    }

        // Same as weak release, but we don't consider whether the last
        // element is p, as that is not valid for strong release.
    case ::spot::ltl::binop::M: {
        // Given q M p:
        // create !p
        const spot::ltl::formula* neg_norm_second =
                spot::ltl::negative_normal_form(node->second(), true);

        // create interval after intvl
        interval temp;
        temp.start = intvl.end;
        //usually we put temp at one before terminal, but if we're at terminal,
        // don't push the end back
        (intvl.end == terminal_pos) ?
                temp.end = terminal_pos : temp.end = terminal_pos - 1;

        // finding the first q in the extended interval
        long next_first_first = find_first_occurrence(node->first(), temp);

        // if q exists
        if (next_first_first != -1) {
            //set temp to check for !p before and including first occ q
            temp.end = next_first_first;
            long last_neg_second = find_last_occurrence(neg_norm_second, temp);
            neg_norm_second->destroy();
            // if !p never occurs we return the end of the interval
            if (last_neg_second == -1)
                return intvl.end;
        }

        // else we check in the interval, finding last q
        long last_first = find_last_occurrence(node->first(), intvl);
        // if there's never a q, return -1;
        if (last_first == -1) {
            return -1;
        }

        temp.start = intvl.start;
        // check to see if p holds at q; if it does, return that q
        if (this->check(node->second(), temp)) {
            return last_first;
        }
        // else if it doesn't hold and we don't have any other choices, -1
        else if (intvl.start == last_first)
            return -1;
        // else check at the next last q
        else {
            intvl.end = last_first - 1;
            return find_last_occurrence(node, intvl);
        }
    }
    default:
        std::cerr << "Unsupported Binary Operation. Returning -1 \n";
        return -1;
    }

}

/**
 * Check all instantiations of the formula on the traces given
 * @param prop_type the property type to check instantiations of
 * @param instantiator to generate all instatiation functions
 * @param traces all the traces
 * @return all valid instantiations
 */
vector<map<string, string>> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<set<map<string_event, vector<long>>> > traces) {
            instantiator->reset_instantiations();
            // vector to return
            vector<map<string, string>> return_vec;
            // create a vector of checkers to retain memoization across instantiation
            // checking.
            vector<map_trace_checker> all_checkers;
            for (set<map<string_event,vector<long>>>::iterator traces_it = traces->begin();
            traces_it != traces->end(); traces_it++) {
                all_checkers.push_back(map_trace_checker(&(*traces_it)));
            }
            int num_traces = all_checkers.size();
            while (true) {
                shared_ptr<map<string,string>> current_instantiation = instantiator->get_next_instantiation();
                if (current_instantiation == NULL) {
                    break;
                }
                const spot::ltl::formula * instantiated_prop_type = instantiate(prop_type,*current_instantiation, instantiator->get_events_to_exclude());
                // is the instantiation valid?
                bool valid = true;
                for (int i = 0; i < num_traces; i++) {
                    bool valid_on_trace = all_checkers[i].check_on_trace(instantiated_prop_type);
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
