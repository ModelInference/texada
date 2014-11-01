/*
 * prefixtreeparser.cpp
 *
 *  Created on: Aug 19, 2014
 *      Author: dennis
 */

#include "prefixtreeparser.h"

namespace texada {

prefix_tree_parser::prefix_tree_parser() {
    trace_id = 0;
    temp_trace = NULL;
    pre_tree_traces = std::make_shared<prefix_tree>();
}

prefix_tree_parser::~prefix_tree_parser() {
}

/**
 * Returns the parsed prefix trees:
 * TODO: guard
 * @return
 */
shared_ptr<prefix_tree> prefix_tree_parser::return_prefix_trees() {
    if (has_been_parsed) {
        return pre_tree_traces;
    } else {
        std::cerr
                << "Trace not parsed into pre trees, returning empty prefix trees. \n";
        return pre_tree_traces;
    }

}

/**
 * Helper function called from within parser::parse(std::ifstream &infile)
 * Adds event to the current trace being built
 * @param event name
 */
void prefix_tree_parser::add_event(string_event event) {
    if (event.is_terminal()) {
        shared_ptr<prefix_tree_node> twin = temp_trace->get_child(event);
        if (twin != NULL) {
            temp_trace->add_id_to_child(trace_id, twin);
            temp_trace = NULL;
        } else {
            //std::cout << "Ending trace " << trace_id << ". \n";
            set<int> trace_ids;
            trace_ids.insert(trace_id);
            shared_ptr<prefix_tree_node> to_insert = std::make_shared<
                    prefix_tree_node>(trace_ids);
            temp_trace->add_child(to_insert);
            temp_trace = NULL;
        }
    } else {
        // if temp_trace is NULL, we just started a new tree.
        if (temp_trace == NULL) {
            trace_id++;
            shared_ptr<prefix_tree_node> trace_start;
            trace_start = pre_tree_traces->get_trace_start(event);
            // todo: what if two -- follow each other. Let's assume they don't
            if (trace_start == NULL) {
                set<int> trace_ids;
                trace_ids.insert(trace_id);
                shared_ptr<prefix_tree_node> to_insert = std::make_shared<
                        prefix_tree_node>(event, trace_ids);
                pre_tree_traces->add_trace(trace_ids, to_insert);
                temp_trace = pre_tree_traces->get_trace_start(event);
                //std::cout << "Added " << line << " in trace " << trace_id << " as beginning of trace. \n";
            } else {
                //std::cout << "Added trace id " << trace_id << " to start event " << line << ".\n";
                pre_tree_traces->add_id_to_trace(trace_id, trace_start);
                temp_trace = trace_start;
            }
        } else {
            // check if this event is already a child of the temp_trace
            shared_ptr<prefix_tree_node> twin = temp_trace->get_child(event);
            if (twin != NULL) {
                //std::cout << "Added trace id " << trace_id << " to event " << line << ".\n";
                temp_trace->add_id_to_child(trace_id,twin);
                temp_trace = twin;
            } else {
                set<int> trace_ids;
                trace_ids.insert(trace_id);
                shared_ptr<prefix_tree_node> to_insert = std::make_shared<
                        prefix_tree_node>(event, trace_ids);
                temp_trace->add_child(to_insert);
                temp_trace = temp_trace->get_child(event);
            }

        }
    }
}

} /* namespace texada */
