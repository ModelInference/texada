/*
 * ltlformulachecker.h
 *
 *  Created on: Aug 15, 2014
 *      Author: clemieux
 */

#ifndef LTLFORMULACHECKER_H_
#define LTLFORMULACHECKER_H_

#include <set>
#include <ltlast/allnodes.hh>
#include <iostream>

namespace texada {

/**
 * Superclass for all property type checkers based on ltl
 * formulae trees
 */
template<typename return_type, typename trace_type>
class ltl_formula_checker {

public:
    ltl_formula_checker() {}
    virtual ~ltl_formula_checker() {}
    // top level check on trace
    virtual bool check_on_trace(const spot::ltl::formula * node,
            trace_type trace_pt)= 0;
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

    virtual return_type check(const spot::ltl::formula* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) {
        switch (node->kind()) {
        case spot::ltl::formula::Constant:
            return check(static_cast<const spot::ltl::constant*>(node), trace_pt, trace_ids);
        case spot::ltl::formula::AtomicProp:
            return check(static_cast<const spot::ltl::atomic_prop*>(node), trace_pt, trace_ids);
        case spot::ltl::formula::UnOp:
            return check(static_cast<const spot::ltl::unop*>(node), trace_pt, trace_ids);
        case spot::ltl::formula::BinOp:
            return check(static_cast<const spot::ltl::binop*>(node), trace_pt, trace_ids);
        case spot::ltl::formula::MultOp:
            return check(static_cast<const spot::ltl::multop*>(node), trace_pt, trace_ids);
        case spot::ltl::formula::BUnOp:
            return check(static_cast<const spot::ltl::bunop*>(node));
        case spot::ltl::formula::AutomatOp:
            return check(static_cast<const spot::ltl::automatop*>(node));
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
    virtual return_type check(const spot::ltl::atomic_prop *node,
                trace_type trace_pt, std::set<int> trace_ids = std::set<int>()){
        return ap_check(node,trace_pt,trace_ids);

    }

    /**
     * spot::ltl::constant is either true, false, or empty word. On true, we return true
     * and on false we return false. Checking an empty word on a trace should probably
     * be true (since if the formula expresses nothing, it should vacuously hold true).
     * @param node: the constant to check
     * @param trace_pt: trace point or interval
     * @param trace_ids: trace ids, if used
     * @return whether node holds on trace
     */
    virtual return_type check(const spot::ltl::constant *node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) {
        spot::ltl::constant::type value = node->val();
        switch (value) {
        case spot::ltl::constant::True:
            return true_check(trace_ids);
        case spot::ltl::constant::False:
            return false_check(trace_ids);
        case spot::ltl::constant::EmptyWord:
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
    virtual return_type check(const spot::ltl::binop *node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) {
        spot::ltl::binop::type opkind = node->op();

        switch (opkind) {

        //XOR case:
        case spot::ltl::binop::Xor: {
            return xor_check(node, trace_pt, trace_ids);
        }
        //Implies case:
        case spot::ltl::binop::Implies: {
            return implies_check(node, trace_pt, trace_ids);
        }
        //Equiv case:
        case spot::ltl::binop::Equiv:{
            return equiv_check(node, trace_pt, trace_ids);
        }
        //Until case:
        case spot::ltl::binop::U:{
            return until_check(node, trace_pt, trace_ids);
        }
        //Release case:
        case spot::ltl::binop::R:{
            return release_check(node, trace_pt, trace_ids);
        }

        //Weak until case:
        case spot::ltl::binop::W:{
            return weakuntil_check(node, trace_pt, trace_ids);
        }

        //Strong release case:
        case spot::ltl::binop::M:{
            return strongrelease_check(node,trace_pt,trace_ids);
        }

        default:
            std::cerr << "Unsupported binary operator: "<< opkind <<". Returning false. \n";
            return false_check(trace_ids);

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
    virtual return_type check(const spot::ltl::unop *node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) {

        spot::ltl::unop::type optype = node->op();

        switch (optype) {

        // Globally case:
        case spot::ltl::unop::G:
            return globally_check(node, trace_pt, trace_ids);

            // Finally case:
        case spot::ltl::unop::F:
            return finally_check(node, trace_pt, trace_ids);

            // Next case:
        case spot::ltl::unop::X:
            return next_check(node, trace_pt, trace_ids);

            // Not case: !p
        case spot::ltl::unop::Not:
            return not_check(node, trace_pt, trace_ids);

            // Other operators are not LTL, don't support them
        default:
            std::cerr << "Unsupported unary operator. Returning false. \n";
            return false_check(trace_ids);

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
    virtual return_type check(const spot::ltl::multop *node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>()) {
        spot::ltl::multop::type opkind = node->op();

        switch (opkind) {

        case spot::ltl::multop::Or: {
            return or_check(node, trace_pt, trace_ids);
        }
        case spot::ltl::multop::And: {
            return and_check(node, trace_pt, trace_ids);
        }
        default:
            std::cerr << "Unsupported multiple operator. Returning false. \n";
            return false_check(trace_ids);

        }

    }

    // unsupported types get default throwing behaviour
    return_type check(const spot::ltl::automatop* node) {
        //TODO: throw
        std::cerr << "Type automatop unsupported. \n";
        return false_check(std::set<int>());
    }
    return_type check(const spot::ltl::bunop* node) {
        //TODO:throw
        std::cerr << "Type bunop unsupported. \n";
        return false_check(std::set<int>());
    }

    // pure virtual methods used in cases.
    virtual return_type false_check(std::set<int> trace_ids =
            std::set<int>())= 0;
    virtual return_type true_check(std::set<int> trace_ids =
            std::set<int>())= 0;
    virtual return_type ap_check(const spot::ltl::atomic_prop* node,
            trace_type trace_pt, std::set<int> trace_ids =
              std::set<int>())= 0;
    virtual return_type xor_check(const spot::ltl::binop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;
    virtual return_type equiv_check(const spot::ltl::binop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;
    virtual return_type implies_check(const spot::ltl::binop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;
    virtual return_type until_check(const spot::ltl::binop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;
    virtual return_type release_check(const spot::ltl::binop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;
    virtual return_type weakuntil_check(const spot::ltl::binop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;
    virtual return_type strongrelease_check(const spot::ltl::binop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;
    virtual return_type globally_check(const spot::ltl::unop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;
    virtual return_type finally_check(const spot::ltl::unop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;
    virtual return_type next_check(const spot::ltl::unop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;
    virtual return_type not_check(const spot::ltl::unop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;
    virtual return_type or_check(const spot::ltl::multop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;
    virtual return_type and_check(const spot::ltl::multop* node,
            trace_type trace_pt, std::set<int> trace_ids = std::set<int>())= 0;

};

} /* namespace texada */

#endif /* LTLFORMULACHECKER_H_ */
