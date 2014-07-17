/*
 * prefixtreenode.h
 *
 *  Created on: Jul 17, 2014
 *      Author: clemieux
 */

#ifndef PREFIXTREENODE_H_
#define PREFIXTREENODE_H_

#include "stringevent.h"

#include <set>
#include <map>
#include <string>

namespace texada {
using std::set;
using std::map;
using std::string;

/**
 * Class representing an event node in a prefix tree.
 */

class prefix_tree_node {
public:
    // terminal constructor
    prefix_tree_node(set<int>);
    // regular constructor
    prefix_tree_node(string,set<int>);
    // destructor
    virtual ~prefix_tree_node();
    // adds a child
    void add_child(prefix_tree_node*);
    // getter for trace_ids
    set<int>* get_trace_ids();
    // getter for name
    string get_name();
    prefix_tree_node* get_child(int);
    int num_children();
private:
    // event this node represents
    string_event event;
    // set of trace_ids this event belongs in
    set<int> trace_ids;
    // map containing the children corresponding
    // to each trace_id. If a single child
    // belongs to many different traces, there is a
    // seperate entry containing that child for each
    // trace.
    map<int,prefix_tree_node*> children;

};

} /* namespace texada */

#endif /* PREFIXTREENODE_H_ */
