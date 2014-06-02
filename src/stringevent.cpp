/*
 * stringevent.cpp
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#include "stringevent.h"

namespace texada {

/**
 * Construct a string event with name and whether it is terminal
 * @param name_ name
 * @param isTerminal_ is this a terminal event?
 */
string_event::string_event(std::string name_, bool isTerminal_) :
	name(name_), isTerminal(isTerminal_){

}

/**
 * Basic constructor to allow for array creation.
 * TODO: possibly add setter methods?
 */
string_event::string_event() :
	name(""), isTerminal(false){

}

string_event::~string_event() {
}

bool string_event::operator<(const string_event& other) const{
	return name < other.get_name();
}

/**
 * Returns the name of the event
 * @return name of event
 */
std::string string_event::get_name() const{
	return name;
}

/**
 * Returns whether the event is terminal
 * @return true if event is terminal
 */
bool string_event::is_terminal() const{
	return isTerminal;
}

} /* namespace texada */
