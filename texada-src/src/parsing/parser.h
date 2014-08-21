/*
 * parser.h
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "stringevent.h"
#include "prefixtree.h"
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

// TODO ib: makes sense to rename this to parser. See comment in
// simpleparser about specializing this parser code through
// inheritance to parse different data structures for Texada.

/**
 * Creates a parser which can parse files in the format returned by
 * invarimint python scripts (one event per line, terminal event is --).
 * Can also return the events
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

    //rename to unique events

};
std::vector<std::string> string_to_args(std::string commands);
}


/* namespace texada */

#endif /* PARSER_H_ */
