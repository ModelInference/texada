/*
 * event.cpp
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#include "event.h"

namespace texada {


/**
 * Construct a non-terminal event string event with name name_.
 * @param name_ name
 */
event::event(std::string name_) {
    props.insert(name_);
    isTerminal = false;
}

/**
 * Basic constructor which creates a terminal event.
 */
event::event() {
    props.insert("EndOfTraceVar");
    isTerminal = true;
}

event::~event() {
}

/**
 * Overrides < operator to allow for set creation.
 * @param other
 * @return
 */
bool event::operator<(const event& other) const {
    return get_props() < other.get_props();
}

/**
 * Overrides == operator.
 * @param other
 * @return
 */
bool event::operator==(const event& other) const {
    return (get_props() == other.get_props() && isTerminal == other.is_terminal());
}

/**
 * Returns the set of propositions holding at this event
 * @return set of satisfied propositions
 */
std::set<std::string> event::get_props() const {
    return props;
}

/**
 * Returns whether a given atomic proposition holds at this event.
 * @param prop the proposition to check
 * @return true if the proposition holds
 */
bool event::is_satisfied(std::string prop) const {
    return props.find(prop) != props.end();
}

/**
 * Returns whether the event is terminal
 * @return true if event is terminal
 */
bool event::is_terminal() const {
    return isTerminal;
}

/**
 * Adds a proposition to the collection of atomic props holding at this event.
 */
void event::add_prop(std::string prop) {
    props.insert(prop);
}

/**
 * Sets this to a terminal event if is_term is true,
 * sets this to a non-terminal event if is_term is false.s
 * @param is_term
 */
void event::set_terminal(bool is_term) {
    isTerminal = is_term;
}

/**
 * Clear away the propositions associated with this event, and set the event to be non-terminal.
 */
void event::clear() {
    props.clear();
    isTerminal = false;
}

} /* namespace texada */
