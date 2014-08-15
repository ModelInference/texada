/*
 * boolbasedchecker.h
 *
 *  Created on: Aug 15, 2014
 *      Author: clemieux
 */

#ifndef BOOLBASEDCHECKER_H_
#define BOOLBASEDCHECKER_H_

#include "ltlformulachecker.h"

namespace texada {

/**
 * superclass for bool-based ltl formula checkers
 */
template <typename T>
class bool_based_checker : public ltl_formula_checker<bool,T> {
public:
    bool_based_checker() {}
    virtual ~bool_based_checker() {}

protected:
    /**
     * Return false.
     * @param trace_ids
     * @return
     */
    virtual bool false_check(std::set<int> trace_ids) {
        return false;
    }

    /**
     * Return true
     * @param trace_ids
     * @return
     */
    virtual bool true_check(std::set<int> trace_ids) {
        return true;
    }

    /**
     * Give p xor q, p xor q is true iff p & !q or !p & q.
     * @param node xor to check
     * @param trace_pt
     * @param trace_ids
     * @return
     */
    virtual bool xor_check(const spot::ltl::binop* node, T trace_pt,
            std::set<int> trace_ids) {
        const spot::ltl::formula * p = node->first();
        const spot::ltl::formula * q = node->second();
        bool qholds = this->check(q, trace_pt);
        return (this->check(p, trace_pt)) ? !qholds : qholds;
    }

    /**
     * Given p <-> q, it's true iff p & q or !p&!q
     * @param node <-> to check
     * @param trace_pt
     * @param trace_ids
     * @return
     */
    virtual bool equiv_check(const spot::ltl::binop* node,
            T trace_pt, std::set<int> trace_ids) {
        const spot::ltl::formula * p = node->first();
        const spot::ltl::formula * q = node->second();
        bool qholds = this->check(q, trace_pt);
        return this->check(p, trace_pt) ? qholds : !qholds;
    }

    /**
     * p -> q is true if p is false or q is true
     * @param node -> to check
     * @param trace_pt
     * @param trace_ids
     * @return
     */
    virtual bool implies_check(const spot::ltl::binop* node,
            T trace_pt, std::set<int> trace_ids) {
        const spot::ltl::formula * p = node->first();
        const spot::ltl::formula * q = node->second();
        return (!this->check(p, trace_pt)) ? true : this->check(q, trace_pt);

    }

    /**
     * !p will be true if p is not true on this trace
     * @param node: the atomic proposition to check
     * @param trace: pointer to the start of the trace
     * @param trace_ids
     * @return
     */
    virtual bool not_check(const spot::ltl::unop* node, T trace_pt,
            std::set<int> trace_ids) {
        const spot::ltl::formula * p = node->child();
        return !this->check(p, trace_pt);
    }

    /**
     * and will be true if all and's children are true on this trace.
     * @param node: the atomic proposition to check
     * @param trace: pointer to the start of the trace
     * @param trace_ids
     * @return
     */
    virtual bool and_check(const spot::ltl::multop* node, T trace_pt,
            std::set<int> trace_ids) {
        int numkids = node->size();
        // for each of the and's children, we check if it is false: if it is,
        // we short circuit and return false. If we have not returned by the
        // end of the loop, then none of the children were false and we return
        // true.
        for (int i = 0; i < numkids; i++) {
            if (!this->check(node->nth(i), trace_pt)) {
                return false;
            }
        }
        return true;

    }

    /**
     * or will be true if one of or's children are true on this trace.
     * @param node: the atomic proposition to check
     * @param trace: pointer to the start of the trace
     * @param trace_ids
     * @return
     */
    virtual bool or_check(const spot::ltl::multop* node, T trace_pt,
            std::set<int> trace_ids) {
        int numkids = node->size();
        // for each of the or's children, we check if it is true: if it is,
        // we short circuit and return true. If we have not returned by the
        // end of the loop, then none of the children were true and we return
        // false.
        for (int i = 0; i < numkids; i++) {
            if (this->check(node->nth(i), trace_pt)) {
                return true;
            }
        }
        return false;

    }

};

} /* namespace texada */

#endif /* BOOLBASEDCHECKER_H_ */
