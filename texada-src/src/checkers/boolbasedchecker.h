/*
 * boolbasedchecker.h
 *
 *  Created on: Aug 15, 2014
 *      Author: clemieux
 */

#ifndef BOOLBASEDCHECKER_H_
#define BOOLBASEDCHECKER_H_

#include "ltlformulachecker.h"
#include "statistic.h"

namespace texada {

/**
 * superclass for bool-based ltl formula checkers
 */
template<typename T>
class bool_based_checker: public ltl_formula_checker<statistic, T> {
public:
    bool_based_checker() {
    }
    virtual ~bool_based_checker() {
    }

protected:
    /**
     * Return false.
     * sup(true) = sup_pot(true) = 0
     * @param trace_ids
     * @return
     */
    virtual statistic false_check(std::set<int> trace_ids) {
        return statistic(false, 0, 0);
    }

    /**
     * Return true.
     * sup(false) = sup_pot(false) = 0
     * @param trace_ids
     * @return
     */
    virtual statistic true_check(std::set<int> trace_ids) {
        return statistic(true, 0, 0);
    }

    /**
     * Give p xor q, p xor q is true iff p & !q or !p & q.
     * Note: Unless the checker is in the vanilla setting, xor will
     * be reduced to the basic operators and so this function
     * will not be called.
     * @param node xor to check
     * @param trace_pt
     * @param trace_ids
     * @return
     */
    virtual statistic xor_check(const spot::ltl::binop* node, T trace_pt,
            std::set<int> trace_ids) {
        const spot::ltl::formula * p = node->first();
        const spot::ltl::formula * q = node->second();
        statistic result_p = this->check(p, trace_pt);
        statistic result_q = this->check(q, trace_pt);
        statistic result = statistic(true, 0, 0);
        result.is_satisfied = (result_p.is_satisfied) ? !result_q.is_satisfied : result_q.is_satisfied;
        return result;
    }

    /**
     * Given p <-> q, it's true iff p & q or !p&!q
     * Note: Unless the checker is in the vanilla setting, <-> will
     * be reduced to the basic operators and so this function
     * will not be called.
     * @param node <-> to check
     * @param trace_pt
     * @param trace_ids
     * @return
     */
    virtual statistic equiv_check(const spot::ltl::binop* node,
            T trace_pt, std::set<int> trace_ids) {
        const spot::ltl::formula * p = node->first();
        const spot::ltl::formula * q = node->second();
        statistic result_p = this->check(p, trace_pt);
        statistic result_q = this->check(q, trace_pt);
        statistic result = statistic(true, 0, 0);
        result.is_satisfied = (result_p.is_satisfied == result_q.is_satisfied);
        return result;
    }

    /**
     * p -> q is true if p is false or q is true
     * Note: Unless the checker is in the vanilla setting, -> will
     * be reduced to the basic operators and so this function
     * will not be called.
     * @param node -> to check
     * @param trace_pt
     * @param trace_ids
     * @return
     */
    virtual statistic implies_check(const spot::ltl::binop* node,
            T trace_pt, std::set<int> trace_ids) {
        const spot::ltl::formula * p = node->first();
        const spot::ltl::formula * q = node->second();
        if (!this->check(p, trace_pt).is_satisfied) {
            return statistic(true, 0, 0);
        } else {
            return this->check(q, trace_pt);
        }
    }

    /**
     * !p will be true if p is not true on this trace
     * Note: Unless the checker is in the vanilla setting, formulae
     * will be reduced to negative normal form so all not operators
     * can be assumed to precede atomic propositions.
     * @param node: the atomic proposition to check
     * @param trace: pointer to the start of the trace
     * @param trace_ids
     * @return
     */
    virtual statistic not_check(const spot::ltl::unop* node, T trace_pt,
            std::set<int> trace_ids) {
        const spot::ltl::formula * p = node->child();
        statistic result_p = this->check(p, trace_pt);
        return (result_p.is_satisfied) ? statistic(false, 0, 1) : statistic(true, 1, 1);
    }

    /**
     * and will be true if all and's children are true on this trace.
     * sup(p_1 and ... and p_n) = sup(p_1) + ... + sup(p_n)
     * sup_pot(p_1 and ... and p_n) = sup_pot(p_1) + ... + sup_pot(p_n)
     * @param node: the atomic proposition to check
     * @param trace: pointer to the start of the trace
     * @param trace_ids
     * @return
     */
    virtual statistic and_check(const spot::ltl::multop* node, T trace_pt,
            std::set<int> trace_ids) {
        int numkids = node->size();
        // In the case that the checker is configured to ignore statistics,
        // for each of the and's children, we check if it is false: if it is,
        // we short circuit and return false. If we have not returned by the
        // end of the loop, then none of the children were false and we return
        // true. If the produced statistics matter, must check each child.

        statistic result = statistic(true, 0, 0);

        for (int i = 0; i < numkids; i++) {
            result = statistic(result, this->check(node->nth(i), trace_pt));
            if (this->conf_threshold == 1.0 && !this->print_stats && !result.is_satisfied) {
                return result;
            }
        }
        return result;

    }

    /**
     * or will be true if one of or's children are true on this trace.
     * sup(p_1 or ... or p_n) = sup(p_k) where interval(p_k) < interval(p_i) for 1<=i<=n
     * sup_pot(p_1 or ... or p_n) defined similarly as above
     * @param node: the atomic proposition to check
     * @param trace: pointer to the start of the trace
     * @param trace_ids
     * @return
     */
    virtual statistic or_check(const spot::ltl::multop* node, T trace_pt,
            std::set<int> trace_ids) {
        int numkids = node->size();
        // In the case that the checker is configured to ignore statistics,
        // for each of the or's children, we check if it is true: if it is,
        // we short circuit and return true. If we have not returned by the
        // end of the loop, then none of the children were true and we return
        // false. If the produced statistics matter, must check each child.

        // TODO

        statistic result = statistic(false, 0, 0);
        statistic result_i;
        for (int i = 0; i < numkids; i++) {
            result_i = this->check(node->nth(i), trace_pt);
            if (result.is_satisfied && result_i.is_satisfied) {
                result = statistic(true, 0, 0);
            } else if (!result.is_satisfied && !result_i.is_satisfied) {
                result = (result.confidence() > result_i.confidence()) ? result : result_i;   // Dennis: not entirely sure about this
            } else {
                result = (result_i.is_satisfied) ? result_i : result;
                if (this->conf_threshold == 1.0 && !this->print_stats && this->sup_threshold == 0 && this->sup_pot_threshold) {
                    return result;
                }
            }
        }
        return result;
    }
};

} /* namespace texada */

#endif /* BOOLBASEDCHECKER_H_ */
