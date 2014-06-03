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
	events = new std::set<std::string>;

}

simple_parser::~simple_parser() {
}

/**
 * Parses the given file into a vector, fills the event set
 * @param filename
 * @return pointer to first element of vector
 */
std::set<std::vector<string_event> > * simple_parser::parse(std::ifstream &infile){
	events->clear();
	std::vector<string_event>* return_vec = new std::vector<string_event>();
	std::set<std::vector<string_event> > * return_set= new std::set<std::vector<string_event> >();
	std::string line;
	while (std::getline(infile,line)){
		// if we're at the end of the trace, add the end of trace variable
		// and exit the loop
		if (line == "--"){
			return_vec->push_back(*(new string_event("EndOfTraceVar", true)));
			return_set->emplace(*return_vec);
			return_vec->clear();
		} else {
			// this is on the very wide assumption that this method
			// is being used on traces with a very precise form
			return_vec->push_back(*(new string_event(line, false)));
			// if the event is not in the set of events, we add it
			if ((events->find(line)==events->end())){
					events->insert(line);
		}

		}


	}
	delete return_vec;
	has_been_parsed = true;
	return return_set;
}

} /* namespace texada */
