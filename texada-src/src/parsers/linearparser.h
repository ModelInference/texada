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
 * parsing of a sequence of log lines into a multiset of vectors, where
 * each vector represents an individual trace. Using a multiset to hold
 * the set of traces allows two things: (i) if there are multiple equivalent
 * traces, an ltl instantiation can be checked over just one of them
 * (ii) if needed, the full statistics of an ltl instantiation can be
 * correctly found by multiplying the statistics found over a trace by
 * the number of equivalent traces.
 */
class linear_parser: public parser {
public:
    linear_parser();
    virtual ~linear_parser();
    shared_ptr<std::multiset<std::vector<string_event> >> return_vec_trace();

private:
    virtual void end_trace();
    virtual void add_event(std::string event);

    vector<string_event> temp_trace;
    shared_ptr<std::multiset<vector<string_event> >> vector_trace_set;
};

} /* namespace texada */

#endif /* LINEARPARSER_H_ */
