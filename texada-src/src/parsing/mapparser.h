/*
 * mapparser.h
 *
 *  Created on: Aug 19, 2014
 *      Author: dennis
 */

#ifndef MAPPARSER_H_
#define MAPPARSER_H_

#include "parser.h"

namespace texada {

class map_parser: public parser {
public:
    map_parser();
    virtual ~map_parser();
    shared_ptr<std::set<std::map<string_event, std::vector<long>> >> return_map_trace();

private:
    virtual void end_trace();
    virtual void add_event(std::string event);

    long pos_count;
    map<string_event, std::vector<long>> temp_trace;
    shared_ptr<set<map<string_event, vector<long>>>> map_trace_set;
};

} /* namespace texada */

#endif /* MAPPARSER_H_ */
