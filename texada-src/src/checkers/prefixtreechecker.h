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

namespace texada {

class prefix_tree_checker {
public:
    prefix_tree_checker();
    virtual ~prefix_tree_checker();

    bool check(const spot::ltl::formula* form_node, shared_ptr<prefix_tree_node> trace_node, int id);

private:
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
