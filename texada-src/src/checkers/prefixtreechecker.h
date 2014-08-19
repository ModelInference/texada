/*
 * prefixtreechecker.h
 *
 *  Created on: Jul 18, 2014
 *      Author: clemieux
 */

#ifndef PREFIXTREECHECKER_H_
#define PREFIXTREECHECKER_H_

#include "../parsing/prefixtree.h"
#include "ltlformulachecker.h"
#include "lineartracechecker.h"
#include <boost/functional/hash.hpp>
#include <boost/unordered_map.hpp>
#include <ltlast/allnodes.hh>
#include <vector>

namespace texada {
using std::vector;
using boost::get;

class prefix_tree_checker: public ltl_formula_checker<map<int, bool>,
        shared_ptr<prefix_tree_node>> {
public:
    prefix_tree_checker();
    virtual ~prefix_tree_checker();
    typedef shared_ptr<prefix_tree_node> trace_node;

    virtual bool check_on_trace(const spot::ltl::formula* node,
            const trace_node trace_pt);

    bool check_on_trace(const spot::ltl::formula* node,
            const trace_node trace_pt, map<string, string>);

    bool check_on_single_trace(const spot::ltl::formula* form_node,
            shared_ptr<prefix_tree_node> trace_node, int id);
    void add_relevant_bindings(
            map<const spot::ltl::formula*, set<string>> * bindings_map);

private:

    bool check_on_single_trace(const spot::ltl::atomic_prop* form_node,
            shared_ptr<prefix_tree_node> trace_node, int id);
    bool check_on_single_trace(const spot::ltl::constant* form_node,
            shared_ptr<prefix_tree_node> trace_node, int id);
    bool check_on_single_trace(const spot::ltl::binop* form_node,
            shared_ptr<prefix_tree_node> trace_node, int id);
    bool check_on_single_trace(const spot::ltl::unop* form_node,
            shared_ptr<prefix_tree_node> trace_node, int id);
    bool check_on_single_trace(const spot::ltl::multop* form_node,
            shared_ptr<prefix_tree_node> trace_node, int id);

    /**
     * both the next types are not supported.
     */
    bool check_on_single_trace(const spot::ltl::automatop* node) {
        std::cerr << "Type automatop unsupported. \n";
        return false;
    }
    bool check_on_single_trace(const spot::ltl::bunop* node) {
        std::cerr << "Type bunop unsupported. \n";
        return false;
    }

    virtual map<int, bool> check_on_kids(const spot::ltl::formula* node,
            map<set<int>, trace_node> trace_pts, set<int> trace_ids);

    map<int, bool> create_int_bool_map(set<int>, bool);

    map<int, bool> not_map(map<int, bool>);

    void add_satisfying_values(map<int, bool>& returned_vals, bool to_satisfy,
            map<int, bool>& map_to_return, set<int>& to_check);

    virtual map<int, bool> false_check(std::set<int> trace_ids);
    virtual map<int, bool> true_check(std::set<int> trace_ids);
    virtual map<int, bool> ap_check(const spot::ltl::atomic_prop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> xor_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> equiv_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> implies_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> until_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> release_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> weakuntil_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> strongrelease_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> globally_check(const spot::ltl::unop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> finally_check(const spot::ltl::unop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> next_check(const spot::ltl::unop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> not_check(const spot::ltl::unop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> or_check(const spot::ltl::multop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int, bool> and_check(const spot::ltl::multop* node,
            trace_node trace_pt, std::set<int> trace_ids);

    // Memo related-stuff:
    bool use_memo = false;
    map<string, string> instantiations;

    struct memo_key {
        const spot::ltl::formula* node;
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
            boost::hash<const spot::ltl::formula *> form_hash;
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

    boost::unordered_map<memo_key, map<int, bool>, hash_memo_key> memo_map;
    map<const spot::ltl::formula*, set<string>> * relevant_bindings_map;

    void add_to_memo_map(const spot::ltl::formula * node, trace_node trace_pt,
            map<int, bool>);
    set<string> aps_of_form(const spot::ltl::formula * node);

};

vector<map<string, string>> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator, shared_ptr<prefix_tree> traces);

} /* namespace texada */

#endif /* PREFIXTREECHECKER_H_ */
