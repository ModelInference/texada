/*
 * parser.h
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "../trace/event.h"
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
 * This abstract class implements basic parsing functions, including
 * methods for configuring and passing in files to be parsed. It also
 * provides a skeletal implementation of the parse function, which
 * calls helper methods that must be implemented in derived
 * classes. Concrete parser classes should implement the helper
 * functions end_trace() and add_event() to maintain a data structure
 * of events and a get function for retrieving this data structure.
 */
class parser {
public:
    parser();
    virtual ~parser();
    virtual void parse(std::ifstream&);

    void set_event_types(std::vector<std::string>);
    void set_trace_separator(std::string);
    void set_event_separator(std::string);
    void ignore_nm_lines();

    shared_ptr<set<string>> return_events();

protected:
    bool has_been_parsed = false;

private:
    shared_ptr<std::string> parse_line(std::string log_line);

    bool get_event(std::ifstream &,event &);
    virtual void add_event(event event);

    shared_ptr<set<string>>  unique_events;
    std::vector<boost::regex> event_types;
    boost::regex trace_separator;
    boost::regex event_separator;
    bool ignores_nm_lines;
    bool parse_mult_prop;

};
}


/* namespace texada */

#endif /* PARSER_H_ */
