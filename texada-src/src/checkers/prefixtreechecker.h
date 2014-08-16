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
#include <boost/variant.hpp>
#include <ltlast/allnodes.hh>
#include <vector>

namespace texada {
using std::vector;
using boost::get;

class prefix_tree_checker : public ltl_formula_checker<map<int,bool>, shared_ptr<prefix_tree_node>> {
public:
    prefix_tree_checker();
    virtual ~prefix_tree_checker();
    typedef shared_ptr<prefix_tree_node> trace_node;

    virtual bool check_on_trace(const spot::ltl::formula* node, const trace_node trace_pt);

    bool check_on_single_trace(const spot::ltl::formula* form_node, shared_ptr<prefix_tree_node> trace_node, int id);

private:

    bool check_on_single_trace(const spot::ltl::atomic_prop* form_node, shared_ptr<prefix_tree_node> trace_node, int id);
    bool check_on_single_trace(const spot::ltl::constant* form_node, shared_ptr<prefix_tree_node> trace_node, int id);
    bool check_on_single_trace(const spot::ltl::binop* form_node, shared_ptr<prefix_tree_node> trace_node, int id);
    bool check_on_single_trace(const spot::ltl::unop* form_node, shared_ptr<prefix_tree_node> trace_node, int id);
    bool check_on_single_trace(const spot::ltl::multop* form_node, shared_ptr<prefix_tree_node> trace_node, int id);

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

    virtual map<int,bool> check_on_kids(const spot::ltl::formula* node, map<set<int>,trace_node> trace_pts, set<int> trace_ids);

    map<int,bool> create_int_bool_map (set<int>, bool);

    map<int,bool> not_map (map<int,bool>);

    void add_satisfying_values(map<int,bool>& returned_vals, bool to_satisfy,
            map<int,bool>& map_to_return,set<int>& to_check);

    virtual map<int,bool> false_check(std::set<int> trace_ids);
    virtual map<int,bool> true_check(std::set<int> trace_ids);
    virtual map<int,bool> ap_check(const spot::ltl::atomic_prop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> xor_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> equiv_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> implies_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> until_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> release_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> weakuntil_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> strongrelease_check(const spot::ltl::binop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> globally_check(const spot::ltl::unop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> finally_check(const spot::ltl::unop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> next_check(const spot::ltl::unop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> not_check(const spot::ltl::unop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> or_check(const spot::ltl::multop* node,
            trace_node trace_pt, std::set<int> trace_ids);
    virtual map<int,bool> and_check(const spot::ltl::multop* node,
            trace_node trace_pt, std::set<int> trace_ids);

};

} /* namespace texada */

#endif /* PREFIXTREECHECKER_H_ */
