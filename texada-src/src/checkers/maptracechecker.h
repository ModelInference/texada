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
#include "../formula/texadatospotmapping.h"
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
    map_trace_checker(const map<event, vector<long>>*, bool, shared_ptr<map<string,string>>);
    virtual ~map_trace_checker();
    statistic check_on_trace(const ltl::formula *, interval intvl = interval());
    statistic check_on_trace(const ltl::formula *, map<string,string>, interval intvl = interval());
    /**
     * This class uses relative positions to check to occurrence of events. As
     * such, it has three extra groups of functions: find first, last and all
     * occurrence(s). These take in the formula and an interval and first the
     * first, last or all occurrence(s) of the formula in that interval.
     */

    // things for memoization
    void add_relevant_bindings(
            map<const ltl::formula*, set<string>> * bindings_map);
    void clear_memo();
    int num_memo_elements();

private:

    struct memoization_key {
        const ltl::formula* formula;
        map<string,string> relevant_instants;
        interval intvl;
        bool operator==(const memoization_key other) const {
            return (formula == other.formula && intvl == other.intvl && relevant_instants == other.relevant_instants);
        }
    };

    struct memoization_key_hash {
        std::size_t operator()(const memoization_key& k) const {
            using boost::hash_value;
            using boost::hash_combine;

            // Start with a hash value of formula.
            std::size_t seed = k.formula->hash();

            // Modify 'seed' by XORing and bit-shifting in
            // one member of 'Key' after the other:

            hash_combine(seed, hash_value(k.relevant_instants));
            hash_combine(seed, hash_value(k.intvl.start));
            hash_combine(seed, hash_value(k.intvl.end));

            // Return the result.
            return seed;
        }
    };
    // position of the terminal event
    long terminal_pos;
    // map from formula interval to the first/last occurrence of the formula in that
    // interval. used in memoization storage.
    std::unordered_map<memoization_key, long, memoization_key_hash> first_occ_map;
    std::unordered_map<memoization_key, long, memoization_key_hash> last_occ_map;
    // the trace this map trace checker checks on
    const map<event, vector<long>> * trace_map;
    // set to true to include memoization 
    bool use_memo; 
    // keeps track of instantiation bindings
    bool use_instant_map;
    map<string,string> instantiation_map;
    // keeps track of which bindings should be stored for each formula node:
    // switch for each different formula checker
    map<const ltl::formula*, set<string>> * relevant_bindings_map;
    // use invariant semantics
    bool use_invariant_semantics;
    shared_ptr<map<string,string>> translations;

    //memoization things
    memoization_key setup_key(const ltl::formula*, interval);
    memoization_key setup_key_ap(const ltl::atomic_prop*, interval);
    long return_and_add_first(const ltl::formula*, interval, long);
    long return_and_add_last(const ltl::formula*, interval, long);
    set<string> aps_of_form(const ltl::formula*);


    virtual statistic ap_check(const ltl::atomic_prop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic until_check(const ltl::binop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic release_check(const ltl::binop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic weakuntil_check(const ltl::binop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic strongrelease_check(const ltl::binop* node,
                interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic globally_check(const ltl::unop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic finally_check(const ltl::unop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());
    virtual statistic next_check(const ltl::unop* node,
            interval intvl, std::set<int> trace_ids = std::set<int>());

    long find_first_occurrence(const ltl::formula*, interval);
    long find_first_occurrence(const ltl::multop*, interval);
    long find_first_occurrence(const ltl::unop*, interval);
    long find_first_occurrence(const ltl::constant*, interval);
    long find_first_occurrence(const ltl::binop*, interval);
    long find_first_occurrence(const ltl::atomic_prop*, interval);


    long find_last_occurrence(const ltl::formula*, interval);
    long find_last_occurrence(const ltl::atomic_prop*, interval);
    long find_last_occurrence(const ltl::multop*, interval);
    long find_last_occurrence(const ltl::unop*, interval);
    long find_last_occurrence(const ltl::constant*, interval);
    long find_last_occurrence(const ltl::binop*, interval);

    /**
     * automatop and bunop are not supported.
     */
    long find_first_occurrence(const ltl::automatop*) {
        throw texada::unsupported_type_exception("Type automatop unsupported.");
    }
    long find_first_occurrence(const ltl::bunop*) {
        throw texada::unsupported_type_exception("Type bunop unsupported.");
    }

    long find_last_occurrence(const ltl::automatop*) {
        throw texada::unsupported_type_exception("Type automatop unsupported.");
    }
    long find_last_occurrence(const ltl::bunop*) {
        throw texada::unsupported_type_exception("Type bunop unsupported.");
    }

};

vector<std::pair<map<string, string>, statistic>> valid_instants_on_traces(
        const ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<set<map<event, vector<long>>>>,
        bool use_invar_semantics = false,
        shared_ptr<map<string,string>> translations = nullptr);
} /* namespace texada */

#endif /* MAPTRACECHECKER_H_ */
