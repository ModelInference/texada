/*
 * prefixtreechecker.h
 *
 *  Created on: Jul 18, 2014
 *      Author: clemieux
 */

#ifndef PREFIXTREECHECKER_H_
#define PREFIXTREECHECKER_H_

#include "../parsing/prefixtree.h"
#include <ltlast/allnodes.hh>
#include <vector>

namespace texada {
using std::vector;

class prefix_tree_checker {
public:
    prefix_tree_checker();
    virtual ~prefix_tree_checker();
    bool check_on_traces(const spot::ltl::formula* form_node, prefix_tree);
    bool check(const spot::ltl::formula* form_node, shared_ptr<prefix_tree_node> trace_node, int id);

private:

    // stores a branch point and the number of
    // its branches that have been visited
    struct branch_point {
        shared_ptr<prefix_tree_node> node;
        int branches_visited;
    };
    // stack storing the last branch points, for backtracking.
    vector<branch_point> bps_to_visit;


    bool check_on_tree(const spot::ltl::formula* form_node, shared_ptr<prefix_tree_node> trace_node);
    bool check(const spot::ltl::atomic_prop* form_node, shared_ptr<prefix_tree_node> trace_node, int id);
    bool check(const spot::ltl::constant* form_node, shared_ptr<prefix_tree_node> trace_node, int id);
    bool check(const spot::ltl::binop* form_node, shared_ptr<prefix_tree_node> trace_node, int id);
    bool check(const spot::ltl::unop* form_node, shared_ptr<prefix_tree_node> trace_node, int id);
    bool check(const spot::ltl::multop* form_node, shared_ptr<prefix_tree_node> trace_node, int id);

    /**
     * both the next types are not supported.
     */
    bool check(const spot::ltl::automatop* node) {
        std::cerr << "Type automatop unsupported. \n";
                return false;
    }
    bool check(const spot::ltl::bunop* node) {
        std::cerr << "Type bunop unsupported. \n";
        return false;
    }

};

} /* namespace texada */

#endif /* PREFIXTREECHECKER_H_ */
