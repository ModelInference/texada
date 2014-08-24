/*
 * parser.h
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "../trace/stringevent.h"
#include "../trace/prefixtree.h"
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <memory>
#include <boost/regex.hpp>

namespace texada {
using std::set;
using std::vector;
using std::string;
using std::map;
using std::shared_ptr;

/**
 * This abstract class implements basic functions of a parser, including
 * methods for configuration and passing in files to be parsed. It also
 * provides a skeletal implementation of the parse function, which calls
 * helper methods to be implemented in derived classes. Concrete parser
 * classes should implement the helper functions end_trace() and add_event()
 * to maintain a data structure of events and a get function for retrieving
 * said data structure.
 */
class parser {
public:
    parser();
    virtual ~parser();
    virtual void parse(std::ifstream &infile);

    void set_event_types(std::vector<std::string> event_types);
    void set_separator(std::string separator_regex);
    void ignore_nm_lines();

    shared_ptr<set<string>> return_events();

protected:
    bool has_been_parsed = false;

private:
    shared_ptr<std::string> get_event_type(std::string log_line);

    virtual void end_trace();
    virtual void add_event(std::string event);

    shared_ptr<set<string>>  unique_events;
    std::vector<boost::regex> event_types;
    boost::regex separator_regex;
    bool ignores_nm_lines;

};
}


/* namespace texada */

#endif /* PARSER_H_ */
