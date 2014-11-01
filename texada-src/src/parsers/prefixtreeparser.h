/*
 * prefixtreeparser.h
 *
 *  Created on: Aug 19, 2014
 *      Author: dennis
 */

#ifndef PREFIXTREEPARSER_H_
#define PREFIXTREEPARSER_H_

#include "parser.h"

namespace texada {

/**
 * This class inherits from the abstract parser class and allows
 * parsing of a sequence of log lines into a prefix tree.
 */
class prefix_tree_parser: public parser {
public:
    prefix_tree_parser();
    virtual ~prefix_tree_parser();
    shared_ptr<prefix_tree> return_prefix_trees();

private:
    virtual void add_event(string_event event);

    int trace_id;
    shared_ptr<prefix_tree_node> temp_trace;
    shared_ptr<prefix_tree> pre_tree_traces;
};

} /* namespace texada */

#endif /* PREFIXTREEPARSER_H_ */
