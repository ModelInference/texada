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
 * represents a string event, with a name and whether or not it is a terminal event
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

	std::string name;
	bool isTerminal;
};

} /* namespace texada */

#endif /* STRINGEVENT_H_ */
