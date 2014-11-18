/*
 * maptracechecker.h
 *
 *  Created on: Jun 10, 2014
 *      Author: clemieux
 */

#ifndef MAPTRACECHECKER_H_
#define MAPTRACECHECKER_H_
#include "../trace/event.h"
#include <map>
#include <unordered_map>
#include <vector>
#include <ltlast/allnodes.hh>
#include <boost/functional/hash.hpp>
#include <boost/unordered_map.hpp>
#include "../instantiation-tools/pregeninstantspool.h"
#include "boolbasedchecker.h"
#include "statistic.h"
#include "interval.h"

namespace texada {

/**
 * Class to check whether an LTL formula holds on a trace in the form of a map.
 * Note that this class uses LONG_MAX as infinity.
 */
class map_trace_checker : public bool_based_checker<interval>{

public:
    map_trace_checker(const map<event, vector<long>>*);
    virtual ~map_trace_checker();
    statistic check_on_trace(const spot::ltl::formula *, interval intvl = interval());
    /**
     * This class uses relative positions to check to occurrence of events. As
     * such, it has three extra groups of functions: find first, last and all
     * occurrence(s). These take in the formula and an interval and first the
     * first, last or all occurrence(s) of the formula in that interval.
     */
private:

    struct formula_interval {
        const spot::ltl::formula* formula;
        interval intvl;
        bool operator==(const formula_interval other) const {
            return (formula == other.formula && intvl == other.intvl);
        }
    };

    struct formula_interval_hash {
        std::size_t operator()(const formula_interval& k) const {
            using boost::hash_value;
            using boost::hash_combine;

            // Start with a hash value of formula.
            std::size_t seed = k.formula->hash();

            // Modify 'seed' by XORing and bit-shifting in
            // one member of 'Key' after the other:

            hash_combine(seed, hash_value(k.intvl.start));
            hash_combine(seed, hash_value(k.intvl.end));

            // Return the result.
            return seed;
        }
    };
    // position of the terminal event
    long terminal_pos;
    // map from formula interval to the first/last occurrence of the formula in that
    // interval. used in memoisation storage.
    std::unordered_map<formula_interval, long, formula_interval_hash> first_occ_map;
    std::unordered_map<formula_interval, long, formula_interval_hash> last_occ_map;
    // the trace this map trace checker checks on
    const map<event, vector<long>> * trace_map;

    virtual statistic ap_check(const spot::ltl::atomic_prop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic until_check(const spot::ltl::binop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic release_check(const spot::ltl::binop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic weakuntil_check(const spot::ltl::binop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic strongrelease_check(const spot::ltl::binop* node,
                interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic globally_check(const spot::ltl::unop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic finally_check(const spot::ltl::unop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic next_check(const spot::ltl::unop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());

    long find_first_occurrence(const spot::ltl::formula*, interval);
    long find_first_occurrence(const spot::ltl::multop*, interval);
    long find_first_occurrence(const spot::ltl::unop*, interval);
    long find_first_occurrence(const spot::ltl::constant*, interval);
    long find_first_occurrence(const spot::ltl::binop*, interval);
    long find_first_occurrence(const spot::ltl::atomic_prop*, interval);

    long return_and_add_first(const spot::ltl::formula*, interval, long);
    long return_and_add_last(const spot::ltl::formula*, interval, long);

    long find_last_occurrence(const spot::ltl::formula*, interval);
    long find_last_occurrence(const spot::ltl::atomic_prop*, interval);
    long find_last_occurrence(const spot::ltl::multop*, interval);
    long find_last_occurrence(const spot::ltl::unop*, interval);
    long find_last_occurrence(const spot::ltl::constant*, interval);
    long find_last_occurrence(const spot::ltl::binop*, interval);

    /**
     * automatop and bunop are not supported.
     */
    long find_first_occurrence(const spot::ltl::automatop*) {
        std::cerr << "Type automatop unsupported. \n";
        return -1;
    }
    long find_first_occurrence(const spot::ltl::bunop*) {
        std::cerr << "Type bunop unsupported. \n";
        return -1;
    }

    long find_last_occurrence(const spot::ltl::automatop*) {
        std::cerr << "Type automatop unsupported. \n";
        return -1;
    }
    long find_last_occurrence(const spot::ltl::bunop*) {
        std::cerr << "Type bunop unsupported. \n";
        return -1;
    }

};

vector<std::pair<map<string, string>, statistic>> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<set<map<event, vector<long>>>>);
} /* namespace texada */

#endif /* MAPTRACECHECKER_H_ */
