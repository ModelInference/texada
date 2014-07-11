/*
 * stringevent.cpp
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#include "stringevent.h"

namespace texada {


/**
 * Construct a non-terminal event string event with name name_.
 * @param name_ name
 */
string_event::string_event(std::string name_) :
        name(name_), isTerminal(false) {
}

/**
 * Basic constructor which creates a terminal event.
 */
string_event::string_event() :
        name("EndOfTraceVar"), isTerminal(true) {
}

string_event::~string_event() {
}

/**
 * Overrides < operator to allow for set creation.
 * @param other
 * @return
 */
bool string_event::operator<(const string_event& other) const {
    return name < other.get_name();
}

/**
 * Overrides == operator.
 * @param other
 * @return
 */
bool string_event::operator==(const string_event& other) const {
    return (name == other.get_name() && isTerminal == other.is_terminal());
}

/**
 * Returns the name of the event
 * @return name of event
 */
std::string string_event::get_name() const {
    return name;
}

/**
 * Returns whether the event is terminal
 * @return true if event is terminal
 */
bool string_event::is_terminal() const {
    return isTerminal;
}

/**
 * Sets event name to inputted name_.
 * @param name_
 * @return
 */
void string_event::set_name(std::string name_) {
    name = name_;
}

/**
 * Sets this to a terminal event if is_term is true,
 * sets this to a non-terminal event if is_term is false.s
 * @param is_term
 */
void string_event::set_terminal(bool is_term) {
    isTerminal = is_term;
}

} /* namespace texada */
