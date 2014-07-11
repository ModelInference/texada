/*
 * simpleparser.cpp
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#include "simpleparser.h"
#include <vector>
#include <fstream>
#include <iostream>

namespace texada {

simple_parser::simple_parser() {
	unique_events = std::set<std::string>();

}

simple_parser::~simple_parser() {
}

/**
 * Parses the given file into a vector, fills the event set
 * @param filename
 * @return set of vector traces
 */
void simple_parser::parse_to_vector(std::ifstream &infile) {
	unique_events.clear();
	vector<string_event> return_vec;
	string line;
	while (std::getline(infile, line)) {
		// if we're at the end of the trace, add the end of trace variable
		// and exit the loop
		if (line == "--") {
			return_vec.push_back(string_event());
			vector_trace_set.insert(return_vec);
			return_vec.clear();
		} else {
			// assumes one event per line, -- terminates
			// constructor w/o terminal event false/true
			return_vec.push_back(string_event(line));
			// if the event is not in the set of events, we add it
			if ((unique_events.find(line) == unique_events.end())) {
				unique_events.insert(line);
			}

		}

	}

	has_been_parsed_vec = true;
}

/**
 * Parses the given file into maps, fills the event set
 * @param filename
 * @return set of map traces
 */
void simple_parser::parse_to_map(std::ifstream &infile) {
	unique_events.clear();
	map<string_event, std::vector<long>> return_map;
	string line;
	long pos_count = 0;
	while (std::getline(infile, line)) {
		// if we're at the end of the trace, add the end of trace variable
		// and exit the loop
		if (line == "--") {
			std::vector<long> pos_vec;
			pos_vec.push_back(pos_count);
			return_map.emplace(string_event(), pos_vec);
			map_trace_set.insert(return_map);
			return_map.clear();
			pos_count = 0;
		} else {
			if ((return_map.find(string_event(line)) == return_map.end())) {
				unique_events.insert(line);
				std::vector<long> pos_vec;
				pos_vec.push_back(pos_count);
				return_map.emplace(string_event(line), pos_vec);
				pos_count++;
			} else {
				return_map.at(string_event(line)).push_back(pos_count);
				pos_count++;
			}
		}
	}
	has_been_parsed_map = true;
}

/**
 * Returns the set of parsed unique events
 * @return
 */
std::set<std::string> simple_parser::return_events() {
	if (has_been_parsed_vec || has_been_parsed_map) {
		return unique_events;
	} else {
		std::cerr << "No files have been parsed, returning empty set. \n";
		return unique_events;
	}
}

/**
 * Returns the set of parsed vector traces
 * @return
 */
std::set<std::vector<string_event> > simple_parser::return_vec_trace() {
	if (has_been_parsed_vec) {
		return vector_trace_set;
	} else {
		std::cerr << "Trace not parsed into vector, returning empty set. \n";
		return vector_trace_set;
	}

}

/**
 * Returns the set of parsed map traces
 * @return
 */
std::set<std::map<string_event, std::vector<long>> > simple_parser::return_map_trace() {
	if (has_been_parsed_map) {
		return map_trace_set;
	} else {
		std::cerr << "Trace not parsed into map, returning empty set. \n";
		return map_trace_set;
	}
}

} /* namespace texada */
