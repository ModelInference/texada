/*
 * simpleparser.h
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#ifndef SIMPLEPARSER_H_
#define SIMPLEPARSER_H_

#include "stringevent.h"
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <memory>

namespace texada {
using std::set;
using std::vector;
using std::string;
using std::map;
using std::shared_ptr;

/**
 * Creates a simple parser which can parse files in the format returned by
 * invarimint python scripts (one event per line, terminal event is --).
 * Can also return the events
 */
class simple_parser {
public:
	simple_parser();
	virtual ~simple_parser();
	void parse_to_vector(std::ifstream &infile);
	void parse_to_map(std::ifstream &infile);

	shared_ptr<set<string>> return_events();

	std::set<std::vector<string_event> > return_vec_trace();

	std::set<std::map<string_event, std::vector<long>> > return_map_trace();

private:
	//rename to unique events
	set<vector<string_event> > vector_trace_set;
	set<map<string_event, vector<long>>>map_trace_set;
	shared_ptr<set<string>>  unique_events;
	bool has_been_parsed_vec = false;
	bool has_been_parsed_map = false;

};

}
/* namespace texada */

#endif /* SIMPLEPARSER_H_ */
