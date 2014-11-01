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
 * Constructs a single propositional event node with an
 * event having the proposition prop
 * @param prop the proposition corresponding to the node's event
 * @param input_ids traces this event belongs to
 */
prefix_tree_node::prefix_tree_node(std::string prop, set<int> input_ids) {
    this->event = string_event(prop);
    trace_ids = input_ids;
}

/**
 * Constructs an event node with event event and belonging
 * to the traces given in input_ids
 * @param event the event corresponding to this node
 * @param input_ids traces this event belongs to
 */
prefix_tree_node::prefix_tree_node(string_event event, set<int> input_ids) {
    this->event = event;
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
 * Adds a given trace_id to this node.
 * @param trace_id
 */
void prefix_tree_node::add_id(int trace_id){
    trace_ids.insert(trace_id);
}

void prefix_tree_node::add_id_to_child(int trace_id, shared_ptr<prefix_tree_node> child){
    for (map<set<int>,shared_ptr<prefix_tree_node>>::iterator child_it = children.begin();
            child_it != children.end(); ){
        if (child_it->second == child){
            child->add_id(trace_id);
            set<int> new_set(child_it->first);
            new_set.insert(trace_id);
            children.erase(child_it++);
            children.emplace(new_set,child);
        }
        else child_it++;
    }

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
string_event prefix_tree_node::get_event() {
    return event;
}

/**
 * Get the next event in the given trace
 * @param trace_id trace to traverse
 * @return next event in trace with given id
 */
shared_ptr<prefix_tree_node> prefix_tree_node::get_child(int trace_id) {
    for (map<set<int>,shared_ptr<prefix_tree_node>>::iterator kids_it= children.begin();
            kids_it != children.end(); kids_it++) {

        if (kids_it->first.find(trace_id) != kids_it->first.end()) {
            return kids_it->second;
        }
    }
    std::cout << "Did not find child. \n";
    return NULL;

}

/**
 * Get the event after this one with a certain name
 * @param name name of event to find
 * @return event with name name, NULL if no such event exists
 */
shared_ptr<prefix_tree_node> prefix_tree_node::get_child(string_event event) {
    for (map<set<int>,shared_ptr<prefix_tree_node>>::iterator kids_it= children.begin();
            kids_it != children.end(); kids_it++) {
        if ((kids_it->second)->get_event() == event) {
            return kids_it->second;
        }
    }
    return NULL;

}

/**
 * Gets the nth child of this node, starting at 1.
 * @param n child to get
 * @return
 */
shared_ptr<prefix_tree_node> prefix_tree_node::get_nth_child(int n){
    // ordered map assures we won't return same child for different ns
    //counter
    int i = 1;
    for (map<set<int>, shared_ptr<prefix_tree_node>>::iterator child_it = children.begin();
            child_it != children.end(); child_it++){
        if (i == n){
            return child_it->second;
        }
        i++;
    }
    // if we don't have an nth child, return null
    return NULL;

}

/**
 * Get all the children of this node
 * @return
 */
map<set<int>,shared_ptr<prefix_tree_node>> prefix_tree_node::get_children(){
    return children;
}

/**
 * Return the number of children this node has.
 * @return
 */
int prefix_tree_node::num_children() {
    return children.size();
}

/**
 * Return whether a given atomic proposition holds at the event corresponding to this node
 * @param prop the proposition to check
 * @return whether the ap holds
 */
bool prefix_tree_node::is_satisfied(std::string prop) {
    return event.is_satisfied(prop);
}

/**
 * Return whether this is a terminal node
 */

bool prefix_tree_node::is_terminal() {
    return event.is_terminal();
}

} /* namespace texada */

