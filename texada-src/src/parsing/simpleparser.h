/*
 * simpleparser.h
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#ifndef SIMPLEPARSER_H_
#define SIMPLEPARSER_H_

#include "stringevent.h"
#include "prefixtree.h"
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
	void parse_to_pretrees(std::ifstream &infile);

	shared_ptr<set<string>> return_events();

	shared_ptr<std::set<std::vector<string_event> >> return_vec_trace();

	shared_ptr<std::set<std::map<string_event, std::vector<long>> >> return_map_trace();

	shared_ptr<prefix_tree> return_prefix_trees();

private:
	//rename to unique events
	shared_ptr<set<vector<string_event> >> vector_trace_set;
	shared_ptr<set<map<string_event, vector<long>>>> map_trace_set;
	shared_ptr<prefix_tree> pre_tree_traces;
	shared_ptr<set<string>>  unique_events;
	bool has_been_parsed_vec = false;
	bool has_been_parsed_map = false;

};
std::vector<std::string> string_to_args(std::string commands);
}


/* namespace texada */

#endif /* SIMPLEPARSER_H_ */
