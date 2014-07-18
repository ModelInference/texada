/*
 * prefixtree.h
 *
 *  Created on: Jul 17, 2014
 *      Author: clemieux
 */

#ifndef PREFIXTREE_H_
#define PREFIXTREE_H_

#include "prefixtreenode.h"

namespace texada {
/**
 * Prefix tree class representing multiple traces in the
 * form of prefix trees.
 */

class prefix_tree {
public:
    prefix_tree();
    virtual ~prefix_tree();
    void add_trace(set<int>, prefix_tree_node*);
    prefix_tree_node* get_trace_start(int);
    int get_num_traces();
private:
    // contains the start of each trace corresponding
    // to is trace_id
    map<int,prefix_tree_node*> traces;
};

} /* namespace texada */

#endif /* PREFIXTREE_H_ */
