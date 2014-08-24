/*
 * stringevent.h
 *
 *  Created on: Jun 2, 2014
 *      Author: clemieux
 */

#ifndef STRINGEVENT_H_
#define STRINGEVENT_H_

#include <string>

namespace texada {

/**
 * Represents a string event that is parsed from an input log.
 */
class string_event {
public:
    string_event();
    string_event(std::string);
    virtual ~string_event();
    bool operator<(const string_event&) const;
    bool operator==(const string_event& other) const;
    std::string get_name() const;
    bool is_terminal() const;
    void set_name(std::string);
    void set_terminal(bool);

private:
    // The string representation of the event.
    std::string name;

    // Whether or not this is the last event for a trace.
    bool isTerminal;
};

} /* namespace texada */

#endif /* STRINGEVENT_H_ */
