/*
 * prefixtree.cpp
 *
 *  Created on: Jul 17, 2014
 *      Author: clemieux
 */

#include "prefixtree.h"
#include <iostream>

namespace texada {

prefix_tree::prefix_tree() {
    // TODO Auto-generated constructor stub

}

prefix_tree::~prefix_tree() {
}

/**
 * Returns the first event of the trace with given
 * trace_id
 * @param trace_id trace_id of the trace
 * @return first event of the trace
 */
shared_ptr<prefix_tree_node> prefix_tree::get_trace_start(int trace_id) {
    for (map<set<int>, shared_ptr<prefix_tree_node>>::iterator traces_it = traces.begin();
            traces_it != traces.end(); traces_it++) {
        if (traces_it->first.find(trace_id) != traces_it->first.end()) {
            return traces_it->second;
        }
    }
    return NULL;
}

/**
 * Returns the first event of the trace with given start event
 * @param first_event name of desired start event
 * @return first event of the trace, NULL if no such trace
 */
shared_ptr<prefix_tree_node> prefix_tree::get_trace_start(string first_event) {
    if (traces.empty()){
        return NULL;
    }
    for (map<set<int>, shared_ptr<prefix_tree_node>>::iterator traces_it = traces.begin();
            traces_it != traces.end(); traces_it++) {
        if (traces_it->second->get_name() == first_event) {
            return traces_it->second;
        }
    }
    return NULL;
}

/**
 * Adds the trace(s) which start(s) with first_event
 * to this prefix tree
 * @param trace_ids trace_ids of traces beginning with first_event
 * @param first_event the first event of the traces
 */
void prefix_tree::add_trace(set<int> trace_ids, shared_ptr<prefix_tree_node> first_event) {
    traces.emplace(trace_ids, first_event);
}

void prefix_tree::add_id_to_trace(int trace_id, shared_ptr<prefix_tree_node> event){
    for (map<set<int>, shared_ptr<prefix_tree_node>>::iterator traces_it = traces.begin();
            traces_it != traces.end(); ) {
        if (traces_it->second == event) {
            event->add_id(trace_id);
            set<int> newset(traces_it->first);
            traces.erase(traces_it++);
            newset.insert(trace_id);
            traces.emplace(newset,event);
        } else traces_it++;
    }

}

int prefix_tree::get_num_traces() {
    int total_num_traces = 0;
    for (map<set<int>, shared_ptr<prefix_tree_node>>::iterator traces_it = traces.begin();
                traces_it != traces.end(); traces_it++) {
            total_num_traces += traces_it->first.size();
    }
    return total_num_traces;
}

} /* namespace texada */
