/*
 * prefixtree.cpp
 *
 *  Created on: Jul 17, 2014
 *      Author: clemieux
 */

#include "prefixtree.h"

namespace texada {

prefix_tree::prefix_tree() {
    // TODO Auto-generated constructor stub

}

prefix_tree::~prefix_tree() {
    // TODO Auto-generated destructor stub
}

/**
 * Returns the first event of the trace with given
 * trace_id
 * @param trace_id trace_id of the trace
 * @return first event of the trace
 */
prefix_tree_node* prefix_tree::get_trace_start(int trace_id){
    return traces.at(trace_id);
}

/**
 * Adds the trace(s) which start(s) with first_event
 * to this prefix tree
 * @param trace_ids trace_ids of traces beginning with first_event
 * @param first_event the first event of the traces
 */
void prefix_tree::add_trace(set<int> trace_ids, prefix_tree_node* first_event){
    // add each trace starting at first_event to the traces.
    for(set<int>::iterator trace_ids_it = trace_ids.begin(); trace_ids_it != trace_ids.end();
            trace_ids_it++){
        traces.emplace(*trace_ids_it,first_event);
    }

}

} /* namespace texada */
