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
    terminal_point = end_vector[0];

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
bool map_trace_checker::check_on_trace(const spot::ltl::formula* node) {
    //std::cout << "###";
    interval base_interval;
    base_interval.end = terminal_point - 1;
    return check(node, base_interval);

}
/**
 * Checks whether a formula holds on the given interval. Switch method to assign
 * formula to its helper.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::check(const spot::ltl::formula* node, interval intvl) {
    switch (node->kind()) {

    case spot::ltl::formula::Constant:
        return check(static_cast<const spot::ltl::constant*>(node), intvl);
    case spot::ltl::formula::AtomicProp:
        return check(static_cast<const spot::ltl::atomic_prop*>(node), intvl);
    case spot::ltl::formula::UnOp:
        return check(static_cast<const spot::ltl::unop*>(node), intvl);
    case spot::ltl::formula::BinOp:
        return check(static_cast<const spot::ltl::binop*>(node), intvl);
    case spot::ltl::formula::MultOp:
        return check(static_cast<const spot::ltl::multop*>(node), intvl);
    case spot::ltl::formula::BUnOp:
        return check(static_cast<const spot::ltl::bunop*>(node));
    case spot::ltl::formula::AutomatOp:
        return check(static_cast<const spot::ltl::automatop*>(node));
    default:
        return false;
    }

}

/**
 * Checks whether a const formula holds on the given interval.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::check(const spot::ltl::constant* node, interval intvl) {
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
 * Checks whether a atomic proposition holds on the given interval.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::check(const spot::ltl::atomic_prop* node,
        interval intvl) {
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
 * Checks whether a unop formula holds on the given interval.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::check(const spot::ltl::unop* node, interval intvl) {
    //std::cout << "Checking " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
    spot::ltl::unop::type optype = node->op();

    switch (optype) {

    // Globally case
    case spot::ltl::unop::G: {
        const spot::ltl::formula * neg_norm_child =
                spot::ltl::negative_normal_form(node->child(), true);
        long first_occ = find_first_occurrence(neg_norm_child, intvl);
        //std::cout << "First occ of " << spot::ltl::to_string(spot::ltl::negative_normal_form(node->child(),true)) << " over "<<
        //		intvl.start << "-" << intvl.end << " is " << first_occ << ". \n";
        neg_norm_child->destroy();
        if (first_occ == -1)
            return true;
        else
            return false;
    }
        // Finally case
    case spot::ltl::unop::F: {
        long first_occ = find_first_occurrence(node->child(), intvl);
        if (first_occ == -1)
            return false;
        else
            return true;
    }

        // Next case
    case spot::ltl::unop::X: {
        if (intvl.start == intvl.end) {
            std::vector<long> lastevent = trace_map->at(string_event());
            intvl.start = lastevent[0];
            intvl.end = lastevent[0];
            return check(node->child(), intvl);
        }
        intvl.start++;
        return check(node->child(), intvl);
    }

        // Not case
    case spot::ltl::unop::Not: {
        return !(check(node->child(), intvl));
    }

        // Other operators are not LTL, don't support them
    default:
        std::cerr << "Unsupported unary operator. Returning false. \n";
        return false;

    }
}

/**
 * Checks whether a binop formula holds on the given interval.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::check(const spot::ltl::binop *node, interval intvl) {
    //std::cout << "Checking " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
    spot::ltl::binop::type opkind = node->op();

    switch (opkind) {

    //XOr case: if first is true, return true if second is false,
    //if first is false, return true if second is true.
    case spot::ltl::binop::Xor:
        return (check(node->first(), intvl)) ?
                !(check(node->second(), intvl)) : check(node->second(), intvl);

        //Implies case: if first is true, return true if second is true,
        //if first is false, return true.
    case spot::ltl::binop::Implies:
        return check(node->first(), intvl) ? check(node->second(), intvl) : true;

        //Equiv case: if first is true, return true if second is true,
        //if first is false, return true if second is false.
    case spot::ltl::binop::Equiv:
        return check(node->first(), intvl) ?
                check(node->second(), intvl) : !check(node->second(), intvl);

        //Until case
    case spot::ltl::binop::U: {
        long first_occ_second = find_first_occurrence(node->second(), intvl);
        if (first_occ_second == -1) {
            return false;
        }
        const spot::ltl::formula * neg_norm_first =
                spot::ltl::negative_normal_form(node->first(), true);
        long first_occ_neg_first = find_first_occurrence(neg_norm_first, intvl);
        neg_norm_first->destroy();
        if (first_occ_neg_first < first_occ_second) {
            return false;
        } else
            return true;
    }

        //Release case
    case spot::ltl::binop::R: {
        const spot::ltl::formula * neg_norm_second =
                spot::ltl::negative_normal_form(node->second(), true);
        long first_occ_neg_second = find_first_occurrence(neg_norm_second,
                intvl);
        neg_norm_second->destroy();
        long first_occ_first = find_first_occurrence(node->first(), intvl);
        if (first_occ_first == -1) {
            return (first_occ_neg_second == -1);
        } else if (first_occ_neg_second <= first_occ_first) {
            return false;
        } else
            return true;
    }

        //Weak until case: identical to until except base case
    case spot::ltl::binop::W: {
        long first_occ_second = find_first_occurrence(node->second(), intvl);
        const spot::ltl::formula * neg_norm_first =
                spot::ltl::negative_normal_form(node->first(), true);
        long first_occ_neg_first = find_first_occurrence(neg_norm_first, intvl);
        neg_norm_first->destroy();
        if (first_occ_second == -1) {
            return (first_occ_neg_first == -1);
        } else if (first_occ_neg_first < first_occ_second) {
            return false;
        } else
            return true;
    }

        //Strong Release case: dual of weak until, identical to weak release except base
    case spot::ltl::binop::M: {
        const spot::ltl::formula * neg_norm_second =
                spot::ltl::negative_normal_form(node->second(), true);
        long first_occ_neg_second = find_first_occurrence(neg_norm_second,
                intvl);
        neg_norm_second->destroy();
        long first_occ_first = find_first_occurrence(node->first(), intvl);
        if (first_occ_first == -1) {
            return false;
        } else if (first_occ_neg_second <= first_occ_first) {
            return false;
        } else
            return true;

    }

    default:
        std::cerr << "Unsupported binary operator. Returning false. \n";
        return false;

    }
}

/**
 * Checks whether a multop (and or or) formula holds on the given interval.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::check(const spot::ltl::multop* node, interval intvl) {
    //std::cout << "Checking " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
    spot::ltl::multop::type opkind = node->op();

    switch (opkind) {

    case spot::ltl::multop::Or: {
        int numkids = node->size();
        // for each of the multop's children, we check if it is true: if it is,
        // we short circuit and return true. If we have not returned by the
        // end of the loop, then none of the children were true and we return
        // false.
        for (int i = 0; i < numkids; i++) {
            if (check(node->nth(i), intvl)) {
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
            if (!check(node->nth(i), intvl)) {
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
 * Adds info the first_occ_map and return first_occ.
 * @param node formula we have first occ of
 * @param intvl interval on which we found node
 * @param first_occ the first occ of node on the interval intvl
 * @return first_occ
 */
long map_trace_checker::return_and_add(const spot::ltl::formula* node,
        interval intvl, long first_occ) {
    if (first_occ != intvl.start) {
        first_occ_storer storer;
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
        first_occ_storer storer;
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
long map_trace_checker::return_and_add_end(const spot::ltl::formula* node,
        interval intvl, long last_occ) {
    first_occ_storer storer;
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
    //std::cout << "Finding first occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
    // REQUIRES: to_search is sorted. this should be assured earlier on.
    first_occ_storer storer;
    storer.intvl.start = intvl.start;
    storer.intvl.end = intvl.end;
    storer.formula = node;
    std::unordered_map<first_occ_storer, long, first_occ_storer_hash>::iterator it =
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
                    //std::cout << "It's -1. \n";
                    return return_and_add(node, intvl, -1);
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
                        //std::cout << "It's " << to_search[newpos+1] << ".\n";
                        return return_and_add(node, intvl,
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
                        //std::cout << "It's -1. \n";
                        return return_and_add(node, intvl, -1);
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
                //std::cout << "It's " << intvl.start << ".\n";
                return return_and_add(node, intvl, intvl.start);
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
                        //std::cout << "It's " << to_search[newpos] << ".\n";
                        return return_and_add(node, intvl, to_search[newpos]);
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
                        //std::cout << "It's -1.\n";
                        return return_and_add(node, intvl, -1);
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
        //std::cout << "It's -1.\n";
        return return_and_add(node, intvl, -1);
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
    //std::cout << "Finding first occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
    spot::ltl::multop::type opkind = node->op();
    switch (opkind) {

    // Or case: total_first_occ set to max. If any of the desired events
    // occur, they will happen before LONG_MAX, so the clause:
    // if(first_occ != -1 && first_occ < total_first_occ)
    // will be entered. This will set any_occ true so -1 will not be
    // returned. If it remains false we will correctly return -1 instead
    // of LONG_MAX.
    case spot::ltl::multop::Or: {
        int numkids = node->size();
        long total_first_occ = LONG_MAX;
        bool any_occ = false;
        for (int i = 0; i < numkids; i++) {
            long first_occ = find_first_occurrence(node->nth(i), intvl);
            if (first_occ != -1 && first_occ < total_first_occ) {
                any_occ = true;
                //std::cout << "##First occ of " << spot::ltl::to_string(node->nth(i)) << " over " <<
                //		intvl.start << "-" << intvl.end << " is " << first_occ << ". \n";
                total_first_occ = first_occ;
            }
        }
        if (any_occ == false)
            return -1;
        else
            return total_first_occ;
    }

        // We find the first occurrence of each child; if one does not occur,
        // the and never occurs.
        // else we take the last of these positions and check if the and occurs
        // there. If it does not, we go one past and check for the first on that
        // interval.
    case spot::ltl::multop::And: {
        int numkids = node->size();
        long total_first_occ = -1;
        for (int i = 0; i < numkids; i++) {
            long first_occ = find_first_occurrence(node->nth(i), intvl);
            if (first_occ == -1)
                return -1;
            if (first_occ > total_first_occ) {
                total_first_occ = first_occ;
            }
        }
        intvl.start = total_first_occ;
        for (int i = 0; i < numkids; i++) {
            if (!check(node->nth(i), intvl)) {
                if (intvl.start != intvl.end) {
                    intvl.start++;
                    return find_first_occurrence(node, intvl);
                } else
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
    //std::cout << "Finding first occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
    spot::ltl::unop::type optype = node->op();

    switch (optype) {

    //Ideally everything should be in negative normal form so this only runs on
    //individual events, in which case the recursion should not be too brutal.
    case spot::ltl::unop::Not: {
        long first_true = find_first_occurrence(node->child(), intvl);
        // if the expression in the not never occurs or first occurs
        // after the first element, then the first not occurrence is
        // the start of the interval
        if (first_true == -1 || first_true > intvl.start) {
            return intvl.start;
        } else if (intvl.start == intvl.end) {
            return -1;
        } else {
            intvl.start++;
            return find_first_occurrence(node, intvl);
        }
    }

        // Find the first occurrence of the child in the interval of nexts.
        // Return the corresponding event that comes before it. Or -1 if
        // it does not.
    case spot::ltl::unop::X: {
        intvl.start++;
        intvl.end++;
        long first_occ = find_first_occurrence(node->child(), intvl);
        if (first_occ == -1)
            return -1;
        else {
            //std::cout << "#$$Returning first occ of " << spot::ltl::to_string(node) << " is " << first_occ -1 << ".\n";
            return first_occ - 1;
        }
    }

        // Globally: find the last occurrence of the negation; first globally will
        // be after that. We have to take into account the possibility of a negation
        // occurring after the end of the interval as well.
    case spot::ltl::unop::G: {
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_point - 1;
        const spot::ltl::formula * neg_norm_child =
                spot::ltl::negative_normal_form(node->child(), true);
        long last_neg_occ = find_last_occurrence(neg_norm_child, temp);
        neg_norm_child->destroy();
        if (last_neg_occ == -1)
            return intvl.start;
        if (last_neg_occ >= intvl.end)
            return -1;
        else
            return ++last_neg_occ;
    }

        // Finally: given Fp, find the first occurrence of p from the start of the
        // interval to the end of the trace. If it occurs at any time, then the first
        // Fp is at the front of the interval.
    case spot::ltl::unop::F: {
        intvl.end = terminal_point - 1;
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
    //std::cout << "Finding first occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
    spot::ltl::binop::type opkind = node->op();

    switch (opkind) {

    //XOr case: Take the first of each one, unless they're at the same place.
    // Return the smallest of the first. If they're at the same place, we'll
    // need to find the next firsts. Worst case they always occur in a pair.
    case spot::ltl::binop::Xor: {
        long first_occ_first = find_first_occurrence(node->first(), intvl);
        long first_occ_second = find_first_occurrence(node->second(), intvl);
        if (first_occ_first == -1)
            return first_occ_second;
        else if (first_occ_second == -1)
            return first_occ_first;
        else if (first_occ_first < first_occ_second)
            return first_occ_first;
        else if (first_occ_first > first_occ_second)
            return first_occ_second;
        // at this point we know first and second are equal.
        else if (first_occ_second == intvl.end)
            return -1;
        else {
            intvl.start = first_occ_second + 1;
            return find_first_occurrence(node, intvl);
        }
    }

        // Find first of negation of first, and find first of validity of last.
        // return whichever is earlier.
    case spot::ltl::binop::Implies: {
        const spot::ltl::formula * neg_norm_first =
                spot::ltl::negative_normal_form(node->first(), true);
        long first_occ_neg_first = find_first_occurrence(neg_norm_first, intvl);
        neg_norm_first->destroy();
        long first_occ_second = find_first_occurrence(node->second(), intvl);
        if (first_occ_neg_first == -1)
            return first_occ_second;
        else if (first_occ_second == -1)
            return first_occ_neg_first;
        else if (first_occ_neg_first < first_occ_second)
            return first_occ_neg_first;
        else
            return first_occ_second;

    }

        //Equiv case: return first where neither occur, of first where both occur.
    case spot::ltl::binop::Equiv: {
        long first_occ_first = find_first_occurrence(node->first(), intvl);
        long first_occ_second = find_first_occurrence(node->second(), intvl);
        if (first_occ_first != intvl.start && first_occ_second != intvl.start) {
            return intvl.start;
        } else if (first_occ_first == first_occ_second) {
            return first_occ_first;
        } else if (intvl.start >= intvl.end) {
            return -1;
        } else {
            intvl.start++;
            return find_first_occurrence(node, intvl);
        }

    }

        // Until case: Find first occurrence of the second. Find the last negation
        // of the first strictly before the occurrence of the second. The first
        // until will be one after this last occurrence of the negation of the first.

    case spot::ltl::binop::U: {
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_point - 1;
        long first_occ_second = find_first_occurrence(node->second(), temp);
        if (first_occ_second == -1)
            return -1;
        intvl.end = first_occ_second - 1;
        const spot::ltl::formula * neg_norm_first =
                spot::ltl::negative_normal_form(node->first(), true);
        long last_occ_neg_first = find_last_occurrence(neg_norm_first, intvl);
        neg_norm_first->destroy();
        if (last_occ_neg_first == -1)
            return intvl.start;
        if (last_occ_neg_first >= intvl.end) {
            return -1;
        } else
            return ++last_occ_neg_first;
    }

        //Weak until case: identical to until except base case
    case spot::ltl::binop::W: {
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_point - 1;
        long first_occ_second = find_first_occurrence(node->second(), temp);
        long last_occ_neg_first;
        if (first_occ_second == -1) {
            const spot::ltl::formula * neg_norm_first =
                    spot::ltl::negative_normal_form(node->first(), true);
            last_occ_neg_first = find_last_occurrence(neg_norm_first, temp);
            neg_norm_first->destroy();
            if (last_occ_neg_first == -1) {
                return intvl.start;
            } else if (last_occ_neg_first >= intvl.end)
                return -1;
            else
                return ++last_occ_neg_first;
        }
        intvl.end = first_occ_second - 1;
        const spot::ltl::formula * neg_norm_first =
                spot::ltl::negative_normal_form(node->first(), true);
        last_occ_neg_first = find_last_occurrence(neg_norm_first, intvl);
        neg_norm_first->destroy();
        if (last_occ_neg_first == -1)
            return intvl.start;
        else if (last_occ_neg_first >= intvl.end) {
            return -1;
        } else
            return ++last_occ_neg_first;
    }

        // Release case: Find first occurrence of first. Find the last negation of the second
        // before (inclusive) the first occurrence of the first. First is either one
        // after this last occurrence or need to repeat with next first occurrence.
        // If there is no first occurrence of the first, return start if there is never an
        // occurrence of the last also.
    case spot::ltl::binop::R: {
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_point - 1;
        interval search_interval;
        long first_occ_first = find_first_occurrence(node->first(), temp);
        //std::cout << "First occurrence of the first was " << first_occ_first << "\n";
        long last_occ_neg_second;
        if (first_occ_first == -1) {
            const spot::ltl::formula * neg_norm_second =
                    spot::ltl::negative_normal_form(node->second(), true);
            last_occ_neg_second = find_last_occurrence(neg_norm_second, temp);
            neg_norm_second->destroy();
            if (last_occ_neg_second == -1)
                return intvl.start;
            else if (last_occ_neg_second >= intvl.end)
                return -1;
            else
                return ++last_occ_neg_second;
        }
        search_interval.start = intvl.start;
        search_interval.end = first_occ_first;
        const spot::ltl::formula * neg_norm_second =
                spot::ltl::negative_normal_form(node->second(), true);
        last_occ_neg_second = find_last_occurrence(neg_norm_second,
                search_interval);
        neg_norm_second->destroy();
        //std::cout << "Last occurrence of neg second was " << last_occ_neg_second << ": " << intvl.start << "-"<<search_interval.end << "\n";
        if (last_occ_neg_second == search_interval.end) {
            if (intvl.end <= search_interval.end)
                return -1;
            intvl.start = ++search_interval.end;
            return find_first_occurrence(node, intvl);
        }

        else if (last_occ_neg_second == -1) {
            return intvl.start;
        } else {
            return ++last_occ_neg_second;
        }
    }

        // Strong release: identical to release except base case:
    case spot::ltl::binop::M: {
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_point - 1;
        interval search_interval;
        long first_occ_first = find_first_occurrence(node->first(), temp);
        //std::cout << "First occurrence of the first was " << first_occ_first << "\n";
        long last_occ_neg_second;
        if (first_occ_first == -1)
            return -1;
        search_interval.start = intvl.start;
        search_interval.end = first_occ_first;
        const spot::ltl::formula * neg_norm_second =
                spot::ltl::negative_normal_form(node->second(), true);
        last_occ_neg_second = find_last_occurrence(neg_norm_second,
                search_interval);
        neg_norm_second->destroy();
        //std::cout << "Last occurrence of neg second was " << last_occ_neg_second << ": " << intvl.start << "-"<<search_interval.end << "\n";
        //std::cout << "Are we entering the if? " << (last_occ_neg_second == search_interval.end) << ".\n";
        if (last_occ_neg_second == search_interval.end) {
            if (intvl.end <= search_interval.end)
                return -1;
            intvl.start = ++search_interval.end;
            return find_first_occurrence(node, intvl);
        }
        if (last_occ_neg_second == -1) {
            return intvl.start;
        } else
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
    //std::cout << "Finding first occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
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
    //std::cout << "Finding last occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
    // REQUIRES: to_search is sorted. this should be assured earlier on.
    first_occ_storer storer;
    storer.intvl.start = intvl.start;
    storer.intvl.end = intvl.end;
    storer.formula = node;
    std::unordered_map<first_occ_storer, long, first_occ_storer_hash>::iterator it =
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
                    return return_and_add_end(node, intvl, -1);
                }
                //else if the previous is smaller than or equal to the end
                if (to_search[newpos - 1] <= intvl.end) {
                    // if it's not smaller than the start, return. Else we fail to find it
                    if (to_search[newpos - 1] >= intvl.start) {
                        return to_search[newpos - 1];
                    } else {
                        return return_and_add_end(node, intvl, -1);
                    }
                }
                // if it's bigger than intvl.end and the one before was also too big,
                // we need to look at the left side
                right = newpos - 1;
            }

            else if (to_search[newpos] == intvl.end) {
                return return_and_add_end(node, intvl, intvl.end);
            }
            // else to_search[newpos]<intvl.end
            else {
                // if it's the last element
                if (newpos >= to_search.size() - 1) {
                    // and it's larger than the start, then it's the last event
                    if (to_search[newpos] >= intvl.start) {
                        return return_and_add_end(node, intvl,
                                to_search[newpos]);
                    }
                    // else if it's smaller than the start, it's not in the interval
                    else {
                        return return_and_add_end(node, intvl, -1);
                    }
                }
                // else newpos is too small so we have to go to the right side
                left = newpos + 1;
            }
        }
    } catch (std::out_of_range &e) {
        // this means we didn't find the event in the trace map, so it never occurs;
        return return_and_add_end(node, intvl, -1);
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
    //std::cout << "Finding last occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
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
        for (int i = 0; i < numkids; i++) {
            long last_occ = find_last_occurrence(node->nth(i), intvl);
            if (last_occ == -1)
                return -1;
            if (last_occ < total_last_occ) {
                total_last_occ = last_occ;
            }
        }
        interval temp;
        temp.start = total_last_occ;
        temp.end = intvl.end;
        for (int i = 0; i < numkids; i++) {
            if (!check(node->nth(i), temp)) {
                if (intvl.start < temp.start) {
                    intvl.end = total_last_occ - 1;
                    return find_last_occurrence(node, intvl);
                } else
                    return -1;
            }
        }
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
    //std::cout << "Finding last occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
    spot::ltl::unop::type optype = node->op();

    switch (optype) {

    // Not case: find the last occurrence of the child in the interval.
    // If the child is the last event and the interval is one long, then
    // we never have not child. If it's the last event in a longer intvl,
    // we can check for the second-last event by reducing the interval
    // length at the back.
    // Otherwise, if the last occurrence of the child is not the last
    // event, then the last event is the last negation.
    case spot::ltl::unop::Not: {
        long last_child = find_last_occurrence(node->child(), intvl);
        if (last_child == intvl.end) {
            if (intvl.start == intvl.end)
                return -1;
            else {
                intvl.end--;
                return find_last_occurrence(node, intvl);
            }
        } else
            return intvl.end;
    }

        // Next case: find the last occurrence of the next (P). If P never
        // occurs, neither does XP. Else return one before the last occurrence,
        // which will be in the original interval.
    case spot::ltl::unop::X: {
        intvl.start++;
        intvl.end++;
        long last_operand = find_last_occurrence(node->child(), intvl);
        if (last_operand == -1)
            return -1;
        else
            return last_operand - 1;
    }

        // Globally case: find the last occurrence of the negation of the
        // child. If this last occurrence is the last event, we never
        // have G. In any other case, the last event is the last appearance of G.
    case spot::ltl::unop::G: {
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_point - 1;
        const spot::ltl::formula * neg_norm_child =
                spot::ltl::negative_normal_form(node->child(), true);
        long last_neg_child = find_last_occurrence(neg_norm_child, temp);
        neg_norm_child->destroy();
        if (last_neg_child >= intvl.end)
            return -1;
        else
            return intvl.end;
    }

        // Finally case: the last occurrence of the child. This is the last
        // occurrence of "eventually".
    case spot::ltl::unop::F: {
        interval temp;
        temp.start = intvl.start;
        temp.end = terminal_point - 1;
        long last_occ_child = find_last_occurrence(node->child(), intvl);
        if (last_occ_child > intvl.end)
            return intvl.end;
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
    //std::cout << "Finding last occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
    spot::ltl::binop::type opkind = node->op();

    switch (opkind) {

    // Xor case: if both lasts are at the same location, we have a problem.
    // If they are at the same location in a one unit interval, XOR does not
    // occur. Else we know last_second is greater than the start of the
    // interval, so we can set the end of the interval to right
    // before the double occurrence correctly.
    // Else we know they do not occur at the same spot so we return the
    // one that occurs later.
    case spot::ltl::binop::Xor: {
        long last_first = find_last_occurrence(node->first(), intvl);
        long last_second = find_last_occurrence(node->second(), intvl);
        if (last_second == last_first) {
            if (last_second == -1)
                return -1;
            if (last_second == intvl.start)
                return -1;
            else {
                intvl.end = last_second - 1;
                return find_last_occurrence(node, intvl);
            }
        }
        if (last_second > last_first)
            return last_second;
        else
            return last_first;
    }

        // Return the last negation of the first or occurrence of the last.
        // If they are both equal to -1, then the last else will be entered
        // and -1 will be returned.
    case spot::ltl::binop::Implies: {
        const spot::ltl::formula * neg_norm_first =
                spot::ltl::negative_normal_form(node->first(), true);
        long last_neg_first = find_last_occurrence(neg_norm_first, intvl);
        neg_norm_first->destroy();
        long last_second = find_last_occurrence(node->second(), intvl);
        if (last_neg_first > last_second)
            return last_neg_first;
        else
            return last_second;
    }

        // Equiv case: if both are equal, then the last occurrence is at
        // the end (we know that at the end, there is not only one of
        // first or second occurring)
        // Else we just step by 1 because the event before the last could
        // have neither the first nor the second, thus be an occurrence
        // of equiv
    case ::spot::ltl::binop::Equiv: {
        long last_first = find_last_occurrence(node->first(), intvl);
        long last_second = find_last_occurrence(node->second(), intvl);
        if (last_first == last_second)
            return intvl.end;
        else if (last_first == intvl.end || last_second == intvl.end) {
            if (intvl.end == intvl.start)
                return -1;
            else {
                intvl.end--;
                return find_last_occurrence(node, intvl);
            }

        } else
            return intvl.end;

    }

        //There are two possibilities for the last occurrence of p U q on an interval:
        //1. If the last occurrence of q overall is in the interval, then that
        //position in the last occasion of p U q.
        //2. If there is another occurrence of q after the end of the interval, and p
        //holds from the last event of the interval to that q,  then the last event of
        //the interval is the last p U q.
        //So, given p U q, intvl.start and intvl.end. term is the terminal event.
        //Find first occ of q from [intvl.end-term]. If it occurs, find first occ of !p
        //on intvl.end to that first occ. If there is a !p, go check next step. If there
        //is no !p, intvl.end.
        //At this point we know the last occ, if it exists, will be situation 1) above.
        //So find last occ of q on intvl, and return this.
    case ::spot::ltl::binop::U: {
        if (intvl.end < terminal_point - 1) {
            interval temp;
            temp.start = intvl.end;
            temp.end = terminal_point - 1;
            long next_first_second = find_first_occurrence(node->second(),
                    temp);
            if (next_first_second == -1)
                break;
            if (next_first_second == temp.start)
                return temp.start;
            temp.end = next_first_second - 1;
            const spot::ltl::formula* neg_norm_first =
                    spot::ltl::negative_normal_form(node->first(), true);
            long last_first = find_last_occurrence(neg_norm_first, temp);
            neg_norm_first->destroy();
            if (last_first == -1)
                return temp.start;
        }

        return find_last_occurrence(node->second(), intvl);
    }

        // Similar to until, but when q does not occur we have to check if the Gfirst
        // holds.
    case ::spot::ltl::binop::W: {
        const spot::ltl::formula* neg_norm_first =
                spot::ltl::negative_normal_form(node->first(), true);
        if (intvl.end < terminal_point - 1) {
            interval temp;
            temp.start = intvl.end;
            temp.end = terminal_point - 1;

            long next_first_second = find_first_occurrence(node->second(),
                    temp);
            if (next_first_second == -1) {
                if (find_last_occurrence(neg_norm_first, temp) == -1) {
                    neg_norm_first->destroy();
                    return intvl.end;
                } else
                    break;
            }
            if (next_first_second == temp.start)
                return temp.start;
            temp.end = next_first_second - 1;
            long last_first = find_last_occurrence(neg_norm_first, temp);
            if (last_first == -1) {
                neg_norm_first->destroy();
                return temp.start;
            }
        }

        long last_second = find_last_occurrence(node->second(), intvl);
        if (last_second == -1) {
            long last_neg_first = find_last_occurrence(neg_norm_first, intvl);
            neg_norm_first->destroy();
            if (last_neg_first != intvl.end)
                return intvl.end;
            else
                return -1;
        } else
            return last_second;
    }

        // Release case: q R p
        // Similar to weak until, but due to pickiness around having q & p at
        // the release case, recurses.
    case ::spot::ltl::binop::R: {
        const spot::ltl::formula* neg_norm_second =
                spot::ltl::negative_normal_form(node->second(), true);

        interval temp;
        temp.start = intvl.end;
        //usually we put temp
        (intvl.end == terminal_point) ?
                temp.end = terminal_point : temp.end = terminal_point - 1;
        //finding the first q
        long next_first_first = find_first_occurrence(node->first(), temp);
        // if there is no q
        if (next_first_first == -1) {
            // if never !p on that interval we're okay
            if (find_last_occurrence(neg_norm_second, temp) == -1) {
                neg_norm_second->destroy();
                return intvl.end;
            } else
                break;
        }
        //set temp to check for !p before and including first occ q
        temp.end = next_first_first;
        long last_neg_second = find_last_occurrence(neg_norm_second, temp);
        neg_norm_second->destroy();

        // if !p never occurs we put down end of the interval
        if (last_neg_second == -1)
            return intvl.end;

        // else we check in the interval, finding last q
        long last_first = find_last_occurrence(node->first(), intvl);
        // if there's never a q, we know that later on there's a !p; else we would have returned
        // earlier ergo we don't have Gp at any point; return -1.
        if (last_first == -1) {
            return -1;
        }
        temp.start = intvl.start;
        // check to see if p holds at q; if it does, return that q
        if (check(node->second(), temp)) {
            return last_first;
        }
        // else if it doesn't hold and we don't have any other choices, -1
        else if (intvl.start == last_first)
            return -1;
        else {
            intvl.end = last_first - 1;
            return find_last_occurrence(node, intvl);
        }
    }

        // Same as weak release, but we don't consider whether the last
        // element is p, as that is not valid for strong release.
    case ::spot::ltl::binop::M: {
        const spot::ltl::formula* neg_norm_second =
                spot::ltl::negative_normal_form(node->second(), true);
        interval temp;
        temp.start = intvl.end;
        //usually we put temp at one before terminal, but if wer're already there
        (intvl.end == terminal_point) ?
                temp.end = terminal_point : temp.end = terminal_point - 1;
        //finding the first q
        long next_first_first = find_first_occurrence(node->first(), temp);
        // if there is no q
        if (next_first_first == -1) {
            return -1;
        }
        //set temp to check for !p before and including first occ q
        temp.end = next_first_first;
        long last_neg_second = find_last_occurrence(neg_norm_second, temp);
        neg_norm_second->destroy();
        // if !p never occurs we put down end of the interval
        if (last_neg_second == -1)
            return intvl.end;

        // else we check in the interval, finding last q
        long last_first = find_last_occurrence(node->first(), intvl);
        // if there's never a q, return -1;
        if (last_first == -1) {
            return -1;
        }
        temp.start = intvl.start;
        // check to see if p holds at q; if it does, return that q
        if (check(node->second(), temp)) {
            return last_first;
        }
        // else if it doesn't hold and we don't have any other choices, -1
        else if (intvl.start == last_first)
            return -1;
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
 * Checks whether the given instantitations of a formula hold on the
 * given trace -- if they do not, the validity of the function is set
 * to false. The original instantiation array is the one returned.
 * @param instantiations all instantiation function mappings
 * @param formula the LTL formula to instantiate
 * @param trace the trace to check on
 * @return updated instantiations, with invalid ones set to false
 */
shared_ptr<vector<pregen_instants_pool::inst_fxn>> map_trace_checker::check_instants_on_trace(
        shared_ptr<vector<pregen_instants_pool::inst_fxn>> instantiations,
        const spot::ltl::formula* formula) {
    int inst_size = instantiations->size();
    for (int i = 0; i < inst_size; i++) {
        // if it's invalid, ignore
        if (!(instantiations->at(i).valid))
            continue;
        map<string, string> current_map = instantiations->at(i).inst_map;
        const spot::ltl::formula* instantiated_form = instantiate(formula,
                current_map);
        instantiations->at(i).valid = check_on_trace(instantiated_form);
        instantiated_form->destroy();
    }
    return instantiations;

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
                const spot::ltl::formula * instantiated_prop_type = instantiate(prop_type,*current_instantiation);
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
