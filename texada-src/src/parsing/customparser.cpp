/*
 * customparser.cpp
 *
 *  Created on: Aug 15, 2014
 *      Author: dennis
 */

#include "customparser.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

namespace texada {

/**
 * By default, all non-equivalent lines belong to
 * different event types and '--' marks a separation
 * between traces.
 */
custom_parser::custom_parser() {
	event_types.push_back("?<ETYPE>.*");
	separator_regex = "--";
	ignores_nm_lines = false;
}

custom_parser::~custom_parser() {

}

/**
 * Parses the given file into a vector, fills the event set
 * @param filename
 */
void custom_parser::parse_to_vector(std::ifstream &infile) {
	custom_parser::preprocess(infile);
	simple_parser::parse_to_vector(infile);
}

/**
 * Parses the given file into maps, fills the event set
 * @param filename
 */
void custom_parser::parse_to_map(std::ifstream &infile) {
	custom_parser::preprocess(infile);
	simple_parser::parse_to_map(infile);
}

/**
 * Parses the given file into prefix trees, fills the event set
 * @param filename
 */
void custom_parser::parse_to_pretrees(std::ifstream &infile) {
	custom_parser::preprocess(infile);
	simple_parser::parse_to_pretrees(infile);
}

void custom_parser::set_event_types(std::vector<std::string> event_types) {
	// TODO
}

void custom_parser::set_separator(std::string separator_regex) {
	// TODO
}

void custom_parser::ignore_nm_lines() {
	// TODO
}

void custom_parser::preprocess(std::ifstream &infile) {
	// TODO
}

} /* namespace texada */
