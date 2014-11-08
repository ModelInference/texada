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

/**
 * Represents an interval of a trace. Used inside map checker to specify the interval on which to
 * check a given LTL formula. Also used inside boolean checker to describe the interval which is
 * taken into account when checking a given LTL formula (e.g. an atomic proposition p will be
 * evaluated over [0,0], Xp will be evaluated over [1,1], Gp will be evaluated over [0,inf],etc.
 */
typedef struct interval {
    long start;
    long end;

    /**
     * Basic constructor for an interval having value [0,inf]
     */
    interval() :
        start(0),
        end(LONG_MAX) {}

    /**
     * Constructs an interval having given start and end times
     * @param s start time
     * @param e end time
     */
    interval(long s, long e) :
        start(s),
        end(e) {}

    /**
     * Moves an interval i steps to the right
     * @param i the number of steps to move interval
     * @return
     */
    interval& operator+(long i) {
        if (start != LONG_MAX) {
            start++;
        }
        if (end != LONG_MAX) {
            end++;
        }
        return *this;
    }

    /**
     * Overrides < operator.
     * An interval [a,b] is greater than an interval [c,d] if
     * b is greater than c. In other words, [a,b] > [c,d] if
     * [a,b] is completely to the right of [c,d].
     * @param other
     * @return
     */
    bool operator<(const interval other) const {
        return (end < other.start);
    }

    /**
     * Overrides == operator.
     * @param other
     * @return
     */
    bool operator==(const interval other) const {
        return (start == other.start && end == other.end);
    }
} interval;

}

#endif /* INTERVAL_H_ */
