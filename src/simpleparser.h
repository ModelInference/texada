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

namespace texada {

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

	std::set<std::string> return_events();

	std::set<std::vector<string_event> > return_vec_trace();

	std::set<std::map<string_event, std::vector<long>> > return_map_trace();

private:
	//rename to unique events
	//TODO: vector and map sets should be here
	std::set<std::vector<string_event> > vector_trace_set;
	std::set<std::map<string_event, std::vector<long>>>map_trace_set;
	std::set<std::string> unique_events;
	bool has_been_parsed_vec = false;
	bool has_been_parsed_map = false;

};

}
/* namespace texada */

#endif /* SIMPLEPARSER_H_ */
