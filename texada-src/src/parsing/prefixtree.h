/*
 * prefixtree.h
 *
 *  Created on: Jul 17, 2014
 *      Author: clemieux
 */

#ifndef PREFIXTREE_H_
#define PREFIXTREE_H_

#include "prefixtreenode.h"
#include <memory>

namespace texada {
using std::shared_ptr;

/**
 * Prefix tree class representing multiple traces in the
 * form of prefix trees.
 */

class prefix_tree {
public:
    prefix_tree();
    virtual ~prefix_tree();
    void add_trace(set<int>, shared_ptr<prefix_tree_node>);
    void add_id_to_trace(int, shared_ptr<prefix_tree_node>);
    shared_ptr<prefix_tree_node> get_trace_start(int);
    shared_ptr<prefix_tree_node> get_trace_start(string);
    int get_num_traces();
private:
    // contains the start of each trace corresponding
    // to is trace_id
    map<set<int>,shared_ptr<prefix_tree_node>> traces;
};

} /* namespace texada */

#endif /* PREFIXTREE_H_ */
