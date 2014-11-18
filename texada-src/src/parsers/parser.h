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
 * function add_event() to maintain a data structure
 * of events and a get function for retrieving this data structure.
 */
class parser {
public:
    parser();
    virtual ~parser();
    virtual void parse(std::ifstream&);

    void set_prop_types(std::vector<std::string>);
    void set_trace_separator(std::string);
    void parse_mult_prop();
    void ignore_nm_lines();

    shared_ptr<set<string>> return_props();

protected:
    bool has_been_parsed = false;
    shared_ptr<std::string> parse_line(std::string log_line);

    bool get_event(std::ifstream &,event &);
    virtual void add_event(event event);

    // the set of unique propositions in the given log
    shared_ptr<set<string>>  unique_props;
    // the regexes used to parse out propositions from log lines
    std::vector<boost::regex> prop_types;
    // the regex used to determine whether a given log line is a trace separator
    boost::regex trace_separator;
    // the regex used to determine whether a given log line is an event separator
    boost::regex event_separator;
    // whether log lines which fail to match any of the above regexes should be ignored
    bool ignores_nm_lines;
    // whether the log should be treated as multi-propositional
    bool parses_mult_prop;

};
}


/* namespace texada */

#endif /* PARSER_H_ */
