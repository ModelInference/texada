/*
 * interval.h
 *
 *  Created on: Oct 21, 2014
 *      Author: dennis
 */

#ifndef INTERVAL_H_
#define INTERVAL_H_
#include <climits>

namespace texada {

typedef struct interval {
    long start;
    long end;

    interval() :
        start(0),
        end(LONG_MAX) {}

    interval(long s, long e) :
        start(s),
        end(e) {}

    interval& operator+(long i) {
        start = (start == LONG_MAX) ? LONG_MAX : start+i;
        end = (end == LONG_MAX) ? LONG_MAX : end+i;
        return *this;
    }

    /**
     * TODO
     */
    bool operator<(const interval other) const {
        return (end < other.start);
    }

    bool operator==(const interval other) const {
        return (start == other.start && end == other.end);
    }
} interval;

}

#endif /* INTERVAL_H_ */
