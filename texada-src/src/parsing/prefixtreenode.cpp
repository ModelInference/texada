/*
 * prefixtreenode.cpp
 *
 *  Created on: Jul 17, 2014
 *      Author: clemieux
 */

#include "prefixtreenode.h"
#include <assert.h>
#include <iostream>

namespace texada {

/**
 * Constructs a terminal event node belonging to the
 * traces given in input_ids
 * @param input_ids traces this event belongs to
 */
prefix_tree_node::prefix_tree_node(set<int> input_ids) {
    event = string_event();
    trace_ids = input_ids;

}

/**
 * Constructs an event node with name name and belonging
 * to the traces given in input_ids
 * @param name name of this event
 * @param input_ids traces this event belongs to
 */
prefix_tree_node::prefix_tree_node(string name, set<int> input_ids) {
    event = string_event(name);
    trace_ids = input_ids;
}

/**
 * Destructor.
 */
prefix_tree_node::~prefix_tree_node() {

}

/**
 * Add the given child to the map of children. Checks
 * that the child's trace id is contained this's trace_ids.
 * @param child_to_add
 */
void prefix_tree_node::add_child(shared_ptr<prefix_tree_node> child_to_add) {
    // go through the child's trace ids and add to this's map
   children.emplace(child_to_add->get_trace_ids(), child_to_add);


}

/**
 * Get the trace ids of this node
 * @return traces this node belongs to
 */
set<int> prefix_tree_node::get_trace_ids() {
    return trace_ids;
}

/**
 * Get the name of the event this node represents
 * @return name of node
 */
string prefix_tree_node::get_name() {
    return event.get_name();
}

/**
 * Get the next event in the given trace
 * @param trace_id trace to traverse
 * @return next event in trace with given id
 */
shared_ptr<prefix_tree_node> prefix_tree_node::get_child(int trace_id) {
    for (map<set<int>,shared_ptr<prefix_tree_node>>::iterator kids_it= children.begin();
            kids_it != children.end(); kids_it++){
        if (kids_it->first.find(trace_id) != kids_it->first.end()){
            return kids_it->second;
        }
    }
    return NULL;

}

/**
 * Return the number of children this node has.
 * @return
 */
int prefix_tree_node::num_children() {
    return children.size();
}

/**
 * Return whether this is a terminal node
 */

bool prefix_tree_node::is_terminal() {
    return event.is_terminal();
}

} /* namespace texada */

