/*
 * prefixtreenode.h
 *
 *  Created on: Jul 17, 2014
 *      Author: clemieux
 */

#ifndef PREFIXTREENODE_H_
#define PREFIXTREENODE_H_

#include "event.h"

#include <set>
#include <map>
#include <string>
#include <memory>

namespace texada {
using std::set;
using std::map;
using std::string;
using std::shared_ptr;

/**
 * Class representing an event node in a prefix tree.
 */

class prefix_tree_node {
public:
    // terminal constructor
    prefix_tree_node(set<int>);
    // simple constructor by atomic proposition
    prefix_tree_node(std::string,set<int>);
    // regular constructor by event
    prefix_tree_node(event,set<int>);
    // destructor
    virtual ~prefix_tree_node();
    // adds a child
    void add_child(shared_ptr<prefix_tree_node>);
    // adds a trace id to this node
    void add_id(int);
    // adds id to a child
    void add_id_to_child(int, shared_ptr<prefix_tree_node>);
    // getter for trace_ids
    set<int> get_trace_ids();
    // getter for event corresponding to node
    event get_event();
    shared_ptr<prefix_tree_node> get_child(int);
    shared_ptr<prefix_tree_node> get_child(event);
    shared_ptr<prefix_tree_node> get_nth_child(int);
    map<set<int>,shared_ptr<prefix_tree_node>> get_children();
    int num_children();
    bool is_satisfied(std::string);
    bool is_terminal();
private:
    // event this node represents
    event node_event;
    // set of trace_ids this event belongs in
    set<int> trace_ids;
    // map containing the children corresponding
    // to each trace_id. If a single child
    // belongs to many different traces, there is a
    // seperate entry containing that child for each
    // trace.
    map<set<int>,shared_ptr<prefix_tree_node>> children;

};

} /* namespace texada */

#endif /* PREFIXTREENODE_H_ */
