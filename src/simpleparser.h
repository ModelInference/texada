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
	std::set<std::vector<string_event> > parse(std::ifstream &infile);
	/**
	 * Returns set of events in a file if a file has been parsed,
	 * else an empty set.
	 * @return
	 */
	std::set<std::string> return_events(){
		if (has_been_parsed){
			return events;
		} else {
			std::cerr << "No files have been parsed, returning empty set. \n";
			return events;
		}
	}
private:
	std::set<std::string> events;
	bool has_been_parsed = false;

};

} /* namespace texada */

#endif /* SIMPLEPARSER_H_ */
