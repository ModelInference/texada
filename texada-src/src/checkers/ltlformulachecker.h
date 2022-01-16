/*
 * ltlformulachecker.h
 *
 *  Created on: Aug 15, 2014
 *      Author: clemieux
 */

#ifndef LTLFORMULACHECKER_H_
#define LTLFORMULACHECKER_H_

#include <set>
#include "../instantiation-tools/texadatospotmapping.h"
#include "../instantiation-tools/texada_allnodes.h"
#include <iostream>
#include "statistic.h"
#include "../exceptions/unsupportedoperation.h"
#include "../exceptions/unsupportedtype.h"

namespace texada {

/**
 * Superclass for all property type checkers based on ltl
 * formulae trees
 */
    template<typename return_type, typename trace_type>
    class ltl_formula_checker {

    public:
        ltl_formula_checker() { }

        virtual ~ltl_formula_checker() { }

        // top level check on trace
        virtual statistic check_on_trace(const ltl::formula *node,
                                         trace_type trace_pt) = 0;

    protected:

        /**
         * Since SPOT's visitor only takes in one parameter (the formula), the accept functionality
         * cannot be used. Thus we need a function that takes in a general formula and correctly
         * assigns which helper should deal with it.
         * @param node: the formula to check
         * @param trace_pt: trace point or interval
         * @param trace_ids: trace ids, if used
         * @return whether node holds on trace
         */

        virtual return_type check(const ltl::formula *node,
                                  trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) {

	switch (node->kind()) {

                case ltl::formula::Constant:
                    return check(static_cast<const ltl::constant *>(node), trace_pt, trace_ids);
                case ltl::formula::AtomicProp:
                    return check(static_cast<const ltl::atomic_prop *>(node), trace_pt, trace_ids);
                case ltl::formula::UnOp:
                    return check(static_cast<const ltl::unop *>(node), trace_pt, trace_ids);
                case  ltl::formula::BinOp:
                    return check(static_cast<const  ltl::binop *>(node), trace_pt, trace_ids);
                case  ltl::formula::MultOp:
                    return check(static_cast<const  ltl::multop *>(node), trace_pt, trace_ids);
                case  ltl::formula::BUnOp:
                    return check(static_cast<const  ltl::bunop *>(node));
                case  ltl::formula::AutomatOp:
                    return check(static_cast<const  ltl::automatop *>(node));
                default:
                    return false_check(trace_ids);
            }
        }

        /**
         *
         * @param node: the atomic proposition to check
         * @param trace_pt: trace point or interval
         * @param trace_ids: trace ids, if used
         * @return whether node holds on trace
         */
        virtual return_type check(const  ltl::atomic_prop *node,
                                  trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) {
            return ap_check(node, trace_pt, trace_ids);

        }

        /**
         *  ltl::constant is either true, false, or empty word. On true, we return true
         * and on false we return false. Checking an empty word on a trace should probably
         * be true (since if the formula expresses nothing, it should vacuously hold true).
         * @param node: the constant to check
         * @param trace_pt: trace point or interval
         * @param trace_ids: trace ids, if used
         * @return whether node holds on trace
         */
        virtual return_type check(const  ltl::constant *node,
                                  trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) {
             ltl::constant::type value = node->val();
            switch (value) {
                case  ltl::constant::True:
                    return true_check(trace_ids);
                case  ltl::constant::False:
                    return false_check(trace_ids);
                case  ltl::constant::EmptyWord:
                    std::cerr
                    << "We came across the empty word. Returning vacuously true. \n";
                    return true_check(trace_ids);
                default:
                    return false_check(trace_ids);
            }
        }

        /**
         * Checks binary operations. XOR, Implies, Equivalent, Until, Release and Weak Until
         * are supported.
         * Other operators are not supported
         * @param node: the binop to check
         * @param trace_pt: trace point or interval
         * @param trace_ids: trace ids, if used
         * @return whether node holds on trace
         */
        virtual return_type check(const  ltl::binop *node,
                                  trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) {
             ltl::binop::type opkind = node->op();
            switch (opkind) {

                //XOR case:
                case  ltl::binop::Xor: {
                    return xor_check(node, trace_pt, trace_ids);
                }
                    //Implies case:
                case  ltl::binop::Implies: {
                    return implies_check(node, trace_pt, trace_ids);
                }
                    //Equiv case:
                case  ltl::binop::Equiv: {
                    return equiv_check(node, trace_pt, trace_ids);
                }
                    //Until case:
                case  ltl::binop::U: {
                    return until_check(node, trace_pt, trace_ids);
                }
                    //Release case:
                case  ltl::binop::R: {
                    return release_check(node, trace_pt, trace_ids);
                }

                    //Weak until case:
                case  ltl::binop::W: {
                    return weakuntil_check(node, trace_pt, trace_ids);
                }

                    //Strong release case:
                case  ltl::binop::M: {
                    return strongrelease_check(node, trace_pt, trace_ids);
                }

                default:
                    throw texada::unsupported_operation_exception("Unsupported binary operator."); // TODO: Add opkind later

            }
        }

        /**
         * A unary operation is either G, F, X or Not. SPOT also includes some other binary operators
         * which are not supported by this checker.
         * A formula which is globally true must be true on every trace suffix of the given trace.
         * A formula which is finally true must be true on some trace suffix of the given trace.
         * A formula which is next true must be true on the next trace suffix of the given trace.
         * A formula is not true is its negation holds on the current trace.
         * @param node: the atomic proposition to check
         * @param trace: pointer to the start of the trace
         * @return whether node holds on trace
         */
        virtual return_type check(const  ltl::unop *node,
                                  trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) {

             ltl::unop::type optype = node->op();

            switch (optype) {

                // Globally case:
                case  ltl::unop::G:
                    return globally_check(node, trace_pt, trace_ids);

                    // Finally case:
                case  ltl::unop::F:
                    return finally_check(node, trace_pt, trace_ids);

                    // Next case:
                case  ltl::unop::X:
                    return next_check(node, trace_pt, trace_ids);

                    // Not case: !p
                case  ltl::unop::Not:
                    return not_check(node, trace_pt, trace_ids);

                    // Other operators are not LTL, don't support them
                default:
                    throw texada::unsupported_operation_exception("Unsupported unary operator.");

            }
        }

        /**
         * Multop includes or and and. SPOT also has other multops which are not supported.
         * Each element of the and needs to be true on the trace; only one element of the
         * or must be true on the trace.
         * @param node: the atomic proposition to check
         * @param trace: pointer to the start of the trace
         * @return whether node holds on the trace
         */
        virtual return_type check(const  ltl::multop *node,
                                  trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) {
             ltl::multop::type opkind = node->op();

            switch (opkind) {

                case  ltl::multop::Or: {
                    return or_check(node, trace_pt, trace_ids);
                }
                case  ltl::multop::And: {
                    return and_check(node, trace_pt, trace_ids);
                }
                default:
                    throw texada::unsupported_operation_exception("Unsupported multiple operator.");

            }

        }

        // unsupported types get default throwing behaviour
        return_type check(const  ltl::automatop *node) {
            throw texada::unsupported_type_exception("Type automatop unsupported.");
        }

        return_type check(const  ltl::bunop *node) {
            throw texada::unsupported_type_exception("Type bunop unsupported.");
        }

        // pure virtual methods used in cases.
        virtual return_type false_check(std::set<int> trace_ids =
        std::set<int>()) = 0;

        virtual return_type true_check(std::set<int> trace_ids =
        std::set<int>()) = 0;

        virtual return_type ap_check(const  ltl::atomic_prop *node,
                                     trace_type trace_pt, std::set<int> trace_ids =
        std::set<int>()) = 0;

        virtual return_type xor_check(const  ltl::binop *node,
                                      trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

        virtual return_type equiv_check(const  ltl::binop *node,
                                        trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

        virtual return_type implies_check(const  ltl::binop *node,
                                          trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

        virtual return_type until_check(const  ltl::binop *node,
                                        trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

        virtual return_type release_check(const ltl::binop *node,
                                          trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

        virtual return_type weakuntil_check(const ltl::binop *node,
                                            trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

        virtual return_type strongrelease_check(const ltl::binop *node,
                                                trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

        virtual return_type globally_check(const ltl::unop *node,
                                           trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

        virtual return_type finally_check(const ltl::unop *node,
                                          trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

        virtual return_type next_check(const ltl::unop *node,
                                       trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

        virtual return_type not_check(const ltl::unop *node,
                                      trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

        virtual return_type or_check(const ltl::multop *node,
                                     trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

        virtual return_type and_check(const ltl::multop *node,
                                      trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) = 0;

    };

} /* namespace texada */

#endif /* LTLFORMULACHECKER_H_ */
