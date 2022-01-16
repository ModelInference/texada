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
#include "settings.h"
#include "interval.h"
#include "../instantiation-tools/texadatospotmapping.h"
#include "../exceptions/unsupportedoperation.h"

namespace texada {

/**
 * superclass for bool-based ltl formula checkers
 */
    template<typename T>
    class bool_based_checker : public ltl_formula_checker<statistic, T> {
    public:
        /**
         * Constructs a vanilla checker,
         * setting all checker settings to default.
         */
        bool_based_checker() {
            configure(settings());
        }

        virtual ~bool_based_checker() {
        }

        /**
         * Configure checker thresholds and other settings.
         * @param c_settings the set of configuration settings for the checker
         */
        void configure(settings c_settings) {
            this->c_settings = c_settings;
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
        virtual statistic xor_check(const ltl::binop *node, T trace_pt,
                                    std::set<int> trace_ids) {
            const ltl::formula *p = node->first();
            const ltl::formula *q = node->second();
            statistic stat_p = this->check(p, trace_pt);
            statistic stat_q = this->check(q, trace_pt);
            statistic stat = statistic(true, 0, 0);
            if (stat_p.is_satisfied) {
                stat.is_satisfied = !stat_q.is_satisfied;
            } else {
                stat.is_satisfied = stat_q.is_satisfied;
            }
            return stat;
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
        virtual statistic equiv_check(const ltl::binop *node,
                                      T trace_pt, std::set<int> trace_ids) {
            const ltl::formula *p = node->first();
            const ltl::formula *q = node->second();
            statistic stat_p = this->check(p, trace_pt);
            statistic stat_q = this->check(q, trace_pt);
            statistic stat = statistic(true, 0, 0);
            stat.is_satisfied = (stat_p.is_satisfied == stat_q.is_satisfied);
            return stat;
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
        virtual statistic implies_check(const ltl::binop *node,
                                        T trace_pt, std::set<int> trace_ids) {
            const ltl::formula *p = node->first();
            const ltl::formula *q = node->second();
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
        virtual statistic not_check(const ltl::unop *node, T trace_pt,
                                    std::set<int> trace_ids) {
            const ltl::formula *p = node->child();
            statistic stat_p = this->check(p, trace_pt);
            if (stat_p.is_satisfied) {
                return statistic(false, 0, 1);
            } else {
                return statistic(true, 1, 1);
            }
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
        virtual statistic and_check(const ltl::multop *node, T trace_pt,
                                    std::set<int> trace_ids) {
            int numkids = node->size();
            // In the case that the checker is configured to ignore statistics,
            // for each of the and's children, we check if it is false: if it is,
            // we short circuit and return false. If we have not returned by the
            // end of the loop, then none of the children were false and we return
            // true. If the produced statistics matter, must check each child.

            statistic stat = statistic(true, 0, 0);

            for (int i = 0; i < numkids; i++) {
                stat = statistic(stat, this->check(node->nth(i), trace_pt));
                if (is_short_circuiting(stat)) {
                    return stat;
                }
            }
            return stat;

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
        virtual statistic or_check(const ltl::multop *node, T trace_pt,
                                   std::set<int> trace_ids) {
            int numkids = node->size();
            // In the case that the checker is configured to ignore statistics,
            // for each of the or's children, we check if it is true: if it is,
            // we short circuit and return true. If we have not returned by the
            // end of the loop, then none of the children were true and we return
            // false.

            statistic stat = statistic(false, 0, 0);
            statistic stat_i;
            interval intvl(-1, -1);
            interval intvl_i;
            for (int i = 0; i < numkids; i++) {
                stat_i = this->check(node->nth(i), trace_pt);
                intvl_i = get_interval(node->nth(i));
                // if the checker is configured to the vanilla setting, short circuit on the first true child
                if (stat_i.is_satisfied && c_settings.is_vanilla()) {
                    return statistic(true, 1, 1);
                }

                // if a child having an interval earlier than another child's interval
                // is satisfied, the entire property is defined to be vacuously true
                if (intvl_i < intvl) {
                    if (stat_i.is_satisfied) {
                        return statistic(true, 0, 0);
                    }
                }
                    // if a child having an interval earlier than another child's interval
                    // is satisfied, the entire property is defined to be vacuously true.
                    // if it is not satisfied, update the stat and intvl by the child with
                    // the latest interval
                else if (intvl < intvl_i) {
                    if (stat.is_satisfied) {
                        return statistic(true, 0, 0);
                    } else {
                        stat = stat_i;
                        intvl = intvl_i;
                    }
                }
                    // if there exists no child with an interval later all others, evaluate
                    // according to a different scheme taking into account the child with the
                    // greatest confidence and support
                else {
                    if (stat.is_satisfied && stat_i.is_satisfied) {
                        return statistic(true, 0, 0);
                    } else {
                        if (stat.confidence() == stat_i.confidence()) {
                            if (stat.support < stat_i.support) {
                                stat = stat_i;
                            }
                        } else {
                            if (stat.confidence() < stat_i.confidence()) {
                                stat = stat_i;
                            }
                        }
                    }
                }
            }
            return stat;
        }

        /**
         * returns the interval on which a given formula is potentially evaluated over
         * @param node the formula to find the interval of
         * @return
         */
        virtual interval get_interval(const ltl::formula *node) {
            switch (node->kind()) {
                // interval(true) = interval(false) = interval(p) = [0,0]
                case ltl::formula::Constant:
                case ltl::formula::AtomicProp:
                    return interval(0, 0);

                case ltl::formula::UnOp: {
                    const ltl::unop *unode = static_cast<const ltl::unop *>(node);
                    switch (unode->op()) {
                        // interval(Gp) = interval(Fp) = [0,inf+]
                        case ltl::unop::G:
                        case ltl::unop::F:
                            return interval(0, LONG_MAX);
                            // interval(Xp) = interval(p) + 1 (i.e. increment both the start and the end of interval(p))
                        case ltl::unop::X:
                            return get_interval(unode->child()) + 1;
                            // interval(Not(p)) = interval(p)
                        case ltl::unop::Not:
                            return get_interval(unode->child());
                        default:
                            throw texada::unsupported_operation_exception("Unsupported unary operator.");
                    }
                }

                case ltl::formula::BinOp: {
                    const ltl::binop *bnode = static_cast<const ltl::binop *>(node);
                    switch (bnode->op()) {
                        // interval(pUq) = interval(pRq) = interval(pWq) = interval(pMq) = [0,inf+]
                        case ltl::binop::U:
                        case ltl::binop::R:
                        case ltl::binop::W:
                        case ltl::binop::M:
                            return interval(0, LONG_MAX);
                        case ltl::binop::Xor:
                        case ltl::binop::Equiv:
                        case ltl::binop::Implies: {
                            interval intvl(LONG_MAX, -LONG_MAX);
                            interval intvl_i = get_interval(bnode->first());
                            if (intvl.start > intvl_i.start) {
                                intvl.start = intvl_i.start;
                            }
                            if (intvl.end < intvl_i.end) {
                                intvl.end = intvl_i.end;
                            }
                            intvl_i = get_interval(bnode->second());
                            if (intvl.start > intvl_i.start) {
                                intvl.start = intvl_i.start;
                            }
                            if (intvl.end < intvl_i.end) {
                                intvl.end = intvl_i.end;
                            }
                            return intvl;
                        }

                        default:
                            throw texada::unsupported_operation_exception("Unsupported binary operator " + ltl::to_string(node) + ".");
                    }
                }

                case ltl::formula::MultOp: {
                    const ltl::multop *mnode = static_cast<const ltl::multop *>(node);
                    switch (mnode->op()) {
                        // interval(Or{p_i}) = interval(And{p_i}) = [a,b], where a = min{p_i.start} and b = max{p_i.end}
                        case ltl::multop::Or:
                        case ltl::multop::And: {
                            interval intvl(LONG_MAX, -LONG_MAX);
                            interval intvl_i;
                            for (int i = 0; i < mnode->size(); i++) {
                                intvl_i = get_interval(mnode->nth(i));
                                if (intvl.start > intvl_i.start) {
                                    intvl.start = intvl_i.start;
                                }
                                if (intvl.end < intvl_i.end) {
                                    intvl.end = intvl_i.end;
                                }
                            }
                            return intvl;
                        }
                        default:
                            throw texada::unsupported_operation_exception("Unsupported multiple operator.");
                    }
                }

                case ltl::formula::BUnOp:
                case ltl::formula::AutomatOp:
                default:
                    return interval(-1, -1);
            }
        }

        /**
         * Determine whether the given statistics allows the checker to
         * short circuit. Depending on the situation and the current
         * configuration of the checker, short circuiting can occur because
         * an instantiation has been found to be valid or invalid early on
         * in the trace and no further checking can change the result.
         */
        bool is_short_circuiting(statistic stat) {
            if (c_settings.conf_t == 1.0 && !c_settings.compute_full_stats) {
                // short circuit for vanilla setting
                return (stat.support >= c_settings.sup_t &&
                        stat.support_potential >= c_settings.sup_pot_t &&
                        !stat.is_satisfied);
            } else if (c_settings.conf_t == 0.0 && !c_settings.compute_full_stats) {
                // short circuit for 0-conf threshold setting
                return (stat.support >= c_settings.sup_t &&
                        stat.support_potential >= c_settings.sup_pot_t);
            } else {
                // otherwise, no short circuiting
                return false;
            }
        }

        // Checker thresholds and other settings. Determines what it means for an LTL instantiation to be "valid"
        settings c_settings;
    };

} /* namespace texada */

#endif /* BOOLBASEDCHECKER_H_ */
