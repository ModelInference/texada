/*
 * prefixtreechecker.h
 *
 *  Created on: Jul 18, 2014
 *      Author: clemieux
 */

#ifndef PREFIXTREECHECKER_H_
#define PREFIXTREECHECKER_H_

#include "../trace/prefixtree.h"
#include "ltlformulachecker.h"
#include "lineartracechecker.h"
#include <boost/functional/hash.hpp>
#include <boost/unordered_map.hpp>
#include <vector>
#include "statistic.h"
#ifdef SMT_SUPPORT
#include "../invariant-semantics/pptinvariantdecider.h"
#include "../formula/texadatospotmapping.h"
#endif

namespace texada {
using std::vector;
using boost::get;

class prefix_tree_checker: public ltl_formula_checker<map<int, statistic>,
        shared_ptr<prefix_tree_node>> {
public:
    prefix_tree_checker();
    prefix_tree_checker(bool, shared_ptr<map<string,string>>);
    virtual ~prefix_tree_checker();
    typedef shared_ptr<prefix_tree_node> trace_node;

    virtual statistic check_on_trace(const ltl::formula* node,
            const trace_node trace_pt);

    statistic check_on_trace(const ltl::formula* node,
            const trace_node trace_pt, map<string, string>);

    void add_relevant_bindings(
            map<const ltl::formula*, set<string>> * bindings_map);

private:

    /**
     * both the next types are not supported.
     */
    statistic check_on_single_trace(const ltl::automatop* node) {
        throw texada::unsupported_type_exception("Type automatop unsupported.");
    }
    statistic check_on_single_trace(const ltl::bunop* node) {
        throw texada::unsupported_type_exception("Type bunop unsupported.");
    }

    virtual map<int, statistic> check_on_kids(const ltl::formula* node,
            map<set<int>, trace_node> trace_pts, set<int> trace_ids);

    map<int, statistic> create_int_bool_map(set<int>, statistic);   // Dennis: should rename to int_statistic_map

    map<int, statistic> not_map(map<int, statistic>);

    void add_satisfying_values(map<int, statistic>& returned_vals, bool to_satisfy,
            map<int, statistic>& map_to_return, set<int>& to_check);

    virtual map<int, statistic> false_check(std::set<int> trace_ids);
    virtual map<int, statistic> true_check(std::set<int> trace_ids);
    virtual map<int, statistic> ap_check(const ltl::atomic_prop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> xor_check(const ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> equiv_check(const ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> implies_check(const ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> until_check(const ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> release_check(const ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> weakuntil_check(const ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> strongrelease_check(const ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> globally_check(const ltl::unop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> finally_check(const ltl::unop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> next_check(const ltl::unop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> not_check(const ltl::unop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> or_check(const ltl::multop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, statistic> and_check(const ltl::multop* node,
            trace_node trace_pt, std::set<int> trace_ids);

    // Memo related-stuff:
    bool use_memo = false;
    map<string, string> instantiations;

    // use invariant semantics
    bool use_invariant_semantics;
    shared_ptr<map<string,string>> translations;

    struct memo_key {
        const ltl::formula* node;
        map<string, string> relevant_mappings;
        trace_node trace_pt;
        bool operator==(const memo_key other) const {
            return node == other.node
                    && relevant_mappings == other.relevant_mappings
                    && trace_pt == other.trace_pt;
        }
    };

    //making a struct because that's what unordered_map likes.
    struct hash_memo_key {
        std::size_t operator()(const memo_key& k) const {
            boost::hash<const ltl::formula *> form_hash;
            std::size_t seed = form_hash(k.node);
            boost::hash<std::pair<string, string>> map_hash;
            map_hash(*k.relevant_mappings.begin());
            for (map<string, string>::const_iterator it =
                    k.relevant_mappings.begin();
                    it != k.relevant_mappings.end(); it++) {
                boost::hash_combine(seed, map_hash(*it));
            }
            boost::hash<shared_ptr<prefix_tree_node>> ptn_hash;
            boost::hash_combine(seed, ptn_hash(k.trace_pt));
            return seed;

        }

    };

    boost::unordered_map<memo_key, map<int, statistic>, hash_memo_key> memo_map;
    map<const ltl::formula*, set<string>> * relevant_bindings_map;

    map<int,statistic> retrieve_memo (const ltl::formula* node,
            trace_node trace_pt, std::set<int> trace_ids);

    void add_to_memo_map(const ltl::formula * node, trace_node trace_pt,
            map<int, statistic>);
    set<string> aps_of_form(const ltl::formula * node);
    void clear_memo_map();

};

vector<std::pair<map<string, string>, statistic>> valid_instants_on_traces(
        const ltl::formula * prop_type,
        instants_pool_creator * instantiator, shared_ptr<prefix_tree> traces,
        bool use_invar_semantics = false,
        shared_ptr<map<string,string>> translations = nullptr);

} /* namespace texada */

#endif /* PREFIXTREECHECKER_H_ */
