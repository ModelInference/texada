/*
 * linearparser.h
 *
 *  Created on: Aug 19, 2014
 *      Author: dennis
 */

#ifndef LINEARPARSER_H_
#define LINEARPARSER_H_

#include "parser.h"

namespace texada {

/**
 * This class inherits from the abstract parser class and allows
 * parsing of a sequence of log lines into a set of vectors, where
 * each vector represents an individual trace.
 */
class linear_parser: public parser {
public:
    linear_parser();
    virtual ~linear_parser();
    shared_ptr<std::set<std::vector<event> >> return_vec_trace();

private:
    virtual void add_event(event event);

    vector<event> temp_trace;
    shared_ptr<set<vector<event> >> vector_trace_set;
};

} /* namespace texada */

#endif /* LINEARPARSER_H_ */
