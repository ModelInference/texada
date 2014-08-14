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

    virtual map<set<int>,trace_node> get_next_event(const trace_node current_node);
    virtual bool is_terminal (const trace_node current_node);
    virtual string event_name (const trace_node current_node);

};

} /* namespace texada */

#endif /* PREFIXTREECHECKER_H_ */
