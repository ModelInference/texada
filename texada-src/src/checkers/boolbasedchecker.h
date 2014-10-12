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
     * @param trace_ids
     * @return
     */
    virtual statistic false_check(std::set<int> trace_ids) { // Dennis: compute and return sup and sup_pot;  change return type to return support and confidence (or support potential?)
        statistic result;
        result.is_satisfied = false;
        // result.support = 0;
        // result.support_potential = 0;
        return result;
    }

    /**
     * Return true
     * @param trace_ids
     * @return
     */
    virtual statistic true_check(std::set<int> trace_ids) { // Dennis: compute and return sup and sup_pot;  change return type to return support and confidence (or support potential?)
        statistic result;
        result.is_satisfied = true;
        // result.support = 0;
        // result.support_potential = 0;
        return result;
    }

    /**
     * Give p xor q, p xor q is true iff p & !q or !p & q.
     * @param node xor to check
     * @param trace_pt
     * @param trace_ids
     * @return
     */
    virtual statistic xor_check(const spot::ltl::binop* node, T trace_pt, // Dennis: compute and return sup and sup_pot;  change return type to return support and confidence (or support potential?)
            std::set<int> trace_ids) {
        const spot::ltl::formula * p = node->first();
        const spot::ltl::formula * q = node->second();
        statistic result_p = this->check(p, trace_pt);
        statistic result_q = this->check(q, trace_pt);
        statistic result;
        result.is_satisfied = (!result_p.is_satisfied && result_q.is_satisfied)
                || (result_p.is_satisfied && !result_q.is_satisfied);
        // result.support = ?;
        // result.support_potential = ?;
        return result;
    }

    /**
     * Given p <-> q, it's true iff p & q or !p&!q
     * @param node <-> to check
     * @param trace_pt
     * @param trace_ids
     * @return
     */
    virtual statistic equiv_check(const spot::ltl::binop* node, // Dennis: compute and return sup and sup_pot;  change return type to return support and confidence (or support potential?)
            T trace_pt, std::set<int> trace_ids) {
        const spot::ltl::formula * p = node->first();
        const spot::ltl::formula * q = node->second();
        statistic result_p = this->check(p, trace_pt);
        statistic result_q = this->check(q, trace_pt);
        statistic result;
        result.is_satisfied = (result_p.is_satisfied == result_q.is_satisfied);
        // result.support = ?;
        // result.support_potential = ?;
        return result;
    }

    /**
     * p -> q is true if p is false or q is true
     * @param node -> to check
     * @param trace_pt
     * @param trace_ids
     * @return
     */
    virtual statistic implies_check(const spot::ltl::binop* node, // Dennis: compute and return sup and sup_pot;  change return type to return support and confidence (or support potential?)
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
     * @param node: the atomic proposition to check
     * @param trace: pointer to the start of the trace
     * @param trace_ids
     * @return
     */
    virtual statistic not_check(const spot::ltl::unop* node, T trace_pt, // Dennis: compute and return sup and sup_pot;  change return type to return support and confidence (or support potential?)
            std::set<int> trace_ids) {
        const spot::ltl::formula * p = node->child();
        statistic result_p = this->check(p, trace_pt);
        statistic result;
        result.is_satisfied = !result_p.is_satisfied;
        // result.support = ?;
        // result.support_potential = ?;
        return result;
    }

    /**
     * and will be true if all and's children are true on this trace.
     * @param node: the atomic proposition to check
     * @param trace: pointer to the start of the trace
     * @param trace_ids
     * @return
     */
    virtual statistic and_check(const spot::ltl::multop* node, T trace_pt, // Dennis: compute and return sup and sup_pot;  change return type to return support and confidence (or support potential?)
            std::set<int> trace_ids) {
        int numkids = node->size();
        // for each of the and's children, we check if it is false: if it is,
        // we short circuit and return false. If we have not returned by the
        // end of the loop, then none of the children were false and we return
        // true.
        statistic result = statistic(true, 0, 0);
        for (int i = 0; i < numkids; i++) {
            statistic result_i = this->check(node->nth(i), trace_pt);
            result.is_satisfied = (result.is_satisfied && result_i.is_satisfied);
            // result.support = ?;
            // result.support_potential = ?;

            // in vanilla setting, return on first instance of unsatisfiability
            if (this->conf_threshold == 1.0 && !this->print_stats) {
                if (!result_i.is_satisfied)
                    break;
            }
        }
        return result;
    }

    /**
     * or will be true if one of or's children are true on this trace.
     * @param node: the atomic proposition to check
     * @param trace: pointer to the start of the trace
     * @param trace_ids
     * @return
     */
    virtual statistic or_check(const spot::ltl::multop* node, T trace_pt, // Dennis: compute and return sup and sup_pot;  change return type to return support and confidence (or support potential?)
            std::set<int> trace_ids) {
        int numkids = node->size();
        // for each of the or's children, we check if it is true: if it is,
        // we short circuit and return true. If we have not returned by the
        // end of the loop, then none of the children were true and we return
        // false.
        statistic result = statistic(false, 0, 0);
        for (int i = 0; i < numkids; i++) {
            statistic result_i = this->check(node->nth(i), trace_pt);
            result.is_satisfied = (result.is_satisfied || result_i.is_satisfied);
            // result.support = ?;
            // result.support_potential = ?;

            // in vanilla setting, return on first instance of satisfiability
            if (this->conf_threshold == 1.0 && !this->print_stats) {
                if (result_i.is_satisfied)
                    break;
            }
        }
        return result;

    }

};

} /* namespace texada */

#endif /* BOOLBASEDCHECKER_H_ */
