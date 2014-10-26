/*
 * statistic.h
 *
 *  Created on: Oct 4, 2014
 *      Author: dennis
 */

#ifndef STATISTIC_H_
#define STATISTIC_H_

namespace texada {

/**
 * Represents the statistics of a finding that is computed by an LTL checker
 */
typedef struct statistic {
    bool is_satisfied;
    int support;
    int support_potential;

    /**
     * Basic constructor which creates a set of default statistics
     */
    statistic() {
        is_satisfied = true;
        support = 0;
        support_potential = 0;
    }

    /**
     * Combines two statistics to construct their sum.
     * @param p the first statistic to combine
     * @param q the second statistic to combine
     */
    statistic(statistic p, statistic q) {
        is_satisfied = p.is_satisfied && q.is_satisfied;
        support = p.support + q.support;
        support_potential = p.support_potential + q.support_potential;
    }

    /**
     * Constructor which creates a statistic struct from given values.
     * @param is_valid whether the corresponding instantiation was satisfied
     * @param sup the support of the corresponding instantiation
     * @param sup_pot the support potential of the corresponding instantiation
     */
    statistic(bool is_valid, int sup, int sup_pot) {
        is_satisfied = is_valid;
        support = sup;
        support_potential = sup_pot;
    }

    /**
     * Computes the confidence of a finding based on support and support potential
     * @return the computed confidence
     */
    float confidence() const {
        if (support == support_potential) {
            if (is_satisfied) {
                return 1.0;
            } else {
                return 0.0;
            }
        } else {
            return (float) support / (float) support_potential;
        }
    }

    /**
     * Overrides < operator to allow for set creation.
     * @param other
     * @return
     */
    bool operator<(statistic other) const {
        return is_satisfied < other.is_satisfied;
    }

    /**
     * Overrides == operator.
     * @param other
     * @return
     */
    bool operator==(statistic other) const {
        return (is_satisfied == other.is_satisfied);
    }

} statistic;

}

#endif /* STATISTIC_H_ */
