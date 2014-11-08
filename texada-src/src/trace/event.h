/*
 * event.h
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <string>
#include <set>

namespace texada {

/**
 * Represents an event that is parsed from an input log.
 * Essentially, an event consists of the atomic propositions which hold
 * at the given point in time.
 */
class event {
public:
    event();
    event(std::string);
    event(std::set<std::string>);
    virtual ~event();
    bool operator<(const event&) const;
    bool operator==(const event& other) const;
    std::set<std::string> get_props() const;
    bool is_satisfied(std::string) const;
    bool is_terminal() const;
    void add_prop(std::string);
    void set_terminal(bool);
    void clear();

private:
    // The string representation of the event.
    std::set<std::string> props;

    // Whether or not this is the last event for a trace.
    bool isTerminal;
};

} /* namespace texada */

#endif /* EVENT_H_ */
