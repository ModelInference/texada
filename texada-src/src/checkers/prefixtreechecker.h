/*
 * prefixtreechecker.h
 *
 *  Created on: Jul 18, 2014
 *      Author: clemieux
 */

#ifndef PREFIXTREECHECKER_H_
#define PREFIXTREECHECKER_H_

#include "../parsing/prefixtree.h"
#include "lineartracechecker.h"
#include <ltlast/allnodes.hh>
#include <vector>

namespace texada {
using std::vector;

class prefix_tree_checker : public linear_trace_checker {
public:
    prefix_tree_checker();
    virtual ~prefix_tree_checker();
    typedef boost::variant<const string_event*,shared_ptr<prefix_tree_node>> trace_node;

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

    virtual map<int,bool> check(const spot::ltl::formula* node, const trace_node trace_pt, set<int> trace_ids);
    virtual map<int,bool> check_on_kids(const spot::ltl::formula* node, map<set<int>,trace_node> trace_pts, set<int> trace_ids);
    virtual map<int,bool> check(const spot::ltl::atomic_prop* node, const trace_node trace_pt, set<int> trace_ids);
    virtual map<int,bool> check(const spot::ltl::constant* node, const trace_node trace_pt, set<int> trace_ids);
    virtual map<int,bool> check(const spot::ltl::binop* node, const trace_node trace_pt, set<int> trace_ids);
    virtual map<int,bool> check(const spot::ltl::unop* node,  const trace_node trace_pt, set<int> trace_ids);
    virtual map<int,bool> check(const spot::ltl::multop* node,  const trace_node trace_pt, set<int> trace_ids);

    virtual map<set<int>,trace_node> get_next_event(const trace_node current_node);
    virtual set<int> get_trace_ids(const trace_node current_node);
    virtual bool is_terminal (const trace_node current_node);
    virtual string event_name (const trace_node current_node);

    map<int,bool> create_int_bool_map (set<int>, bool);

    map<int,bool> not_map (map<int,bool>);

    void add_satisfying_values(map<int,bool>& returned_vals, bool to_satisfy,
            map<int,bool>& map_to_return,set<int>& to_check);

    /**
     * both the next types are not supported.
     */
    map<int,bool> check(const spot::ltl::automatop* node) {
        std::cerr << "Type automatop unsupported. \n";
                return map<int,bool>();
    }
    map<int,bool> check(const spot::ltl::bunop* node) {
        std::cerr << "Type bunop unsupported. \n";
        return map<int,bool>();
    }

};

} /* namespace texada */

#endif /* PREFIXTREECHECKER_H_ */
