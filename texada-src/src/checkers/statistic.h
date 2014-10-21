/*
 * statistic.h
 *
 *  Created on: Oct 4, 2014
 *      Author: dennis
 */

#ifndef STATISTIC_H_
#define STATISTIC_H_

namespace texada {

typedef struct statistic {
    bool is_satisfied;
    int support;
    int support_potential;
    statistic() :
        is_satisfied(true),
        support(0),
        support_potential(0) {}

    statistic(statistic p, statistic q) :
        is_satisfied(p.is_satisfied && q.is_satisfied),
        support(p.support + q.support),
        support_potential(p.support_potential + q.support_potential) {}

    statistic(bool valid, int sup, int sup_pot) :
        is_satisfied(valid),
        support(sup),
        support_potential(sup_pot) {}

    float confidence() {
        return (support == support_potential) ? 1.0 : (float) support / (float) support_potential;
    }

    bool operator<(statistic other) const
    {
        return is_satisfied < other.is_satisfied;
    }

    bool operator==(statistic other) const
    {
        return (is_satisfied == other.is_satisfied);
    }

} statistic;

}

#endif /* STATISTIC_H_ */
