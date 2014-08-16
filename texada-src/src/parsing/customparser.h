/*
 * customparser.h
 *
 *  Created on: Aug 15, 2014
 *      Author: dennis
 */

#ifndef CUSTOMPARSER_H_
#define CUSTOMPARSER_H_

#include "simpleparser.h"
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
 * Creates a parser which can be configured to parse files of arbitrary
 * format by providing it with regular expressions, which specify the
 * form of different event types and other options.
 */
class custom_parser: public simple_parser {
public:
	custom_parser();
	virtual ~custom_parser();
	virtual void parse_to_vector(std::ifstream &infile);
	virtual void parse_to_map(std::ifstream &infile);
	virtual void parse_to_pretrees(std::ifstream &infile);
	void set_event_types(std::vector<std::string> event_types);
	void set_separator(std::string separator_regex);
	void ignore_nm_lines();

private:
	void preprocess(std::ifstream &infile);

	std::vector<std::string> event_types;
	std::string separator_regex;
	bool ignores_nm_lines;
};

} /* namespace texada */

#endif /* CUSTOMPARSER_H_ */
