/*
 * lineartracechecker.cpp
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 */

#include "lineartracechecker.h"
#include "boolbasedchecker.h"
#include "../instantiation-tools/apsubbingcloner.h"
#include "../instantiation-tools/pregeninstantspool.h"
#include "../formula/texadatospotmapping.h"
#include "../formula/texadasimplify.h"
#include "statistic.h"
#include "settings.h"
namespace texada {

linear_trace_checker::linear_trace_checker(){
    use_invariant_semantics = false;
    translations = nullptr;
}

linear_trace_checker::linear_trace_checker(bool use_inv_s, shared_ptr<map<string,string>> ptr){
    use_invariant_semantics = use_inv_s;
    translations = ptr;
}

statistic linear_trace_checker::check_on_trace(const ltl::formula * node, const event * trace){
    return this->check(node, trace);
}



/**
 * Checking a single event on a trace means we check it on the first element of the trace
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @return whether node holds on trace
 */
statistic linear_trace_checker::ap_check(const ltl::atomic_prop *node,
        const event *trace, std::set<int> trace_ids) {
    if (trace->is_satisfied(node->name())) {
        return statistic(true, 1, 1);
    } else {
#ifdef  SMT_SUPPORT
        if (use_invariant_semantics){
            if (ap_holds(*trace, node->name(),translations)){
                return statistic(true, 1, 1);
            } else{
                return statistic(false, 0, 1);
            }
        } else {
#endif
            return statistic(false, 0, 1);
        }
#ifdef SMT_SUPPORT
}
#endif
}

/**
 * p U q will be true if !p does not occur before (exclusive) q
 * Also, q must occur.
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::until_check(const ltl::binop* node,
        const event* trace_pt, std::set<int> trace_ids) {
    const ltl::formula * p = node->first();
    const ltl::formula * q = node->second();
    
    statistic cur_stat_p;
    statistic cur_stat_q;
    statistic ret_stat = statistic(true, 0, 0);
    
    while (!trace_pt->is_terminal()){
      if ((cur_stat_q = this->check(q, trace_pt)).is_satisfied){
        return statistic(cur_stat_q,ret_stat);
      }
      else if (is_short_circuiting(cur_stat_p = this->check(p, trace_pt))) {
        return statistic(cur_stat_p,ret_stat);
      } else {
        ret_stat = statistic(cur_stat_p, ret_stat);
        trace_pt++;
      }
    }
    
    // Dennis is not entirely sure about this base case (see above)
    return statistic(ret_stat, statistic(false, 0, 1)); 
    
}

/**
 * p R q will be true if !q does not occur before (inclusive) q
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::release_check(const ltl::binop* node,
        const event* trace_pt, std::set<int> trace_ids) {
    const ltl::formula * p = node->first();
    const ltl::formula * q = node->second();

    statistic cur_stat_p;
    statistic cur_stat_q;
    statistic ret_stat = statistic(true, 0, 0);
    
    while (!trace_pt->is_terminal()){
      if (is_short_circuiting(cur_stat_q = this->check(q, trace_pt))){
        return statistic(cur_stat_q,ret_stat);
      }
      else if ((cur_stat_p = this->check(p, trace_pt)).is_satisfied){
        return statistic(cur_stat_q,ret_stat);
      } else {
        ret_stat = statistic(cur_stat_q, ret_stat);
        trace_pt++;
      }
    }
    
    // Dennis is not entirely sure about this base case (see above)
    return statistic(ret_stat, statistic(true, 0, 0)); 

}

/**
 * p M q will be true if !q does not occur before (inclusive) q. q must occur.
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::strongrelease_check(const ltl::binop* node,
        const event* trace_pt, std::set<int> trace_ids) {
    const ltl::formula * p = node->first();
    const ltl::formula * q = node->second();
        
    statistic cur_stat_p;
    statistic cur_stat_q;
    statistic ret_stat = statistic(true, 0, 0);
    
    while (!trace_pt->is_terminal()){
      if (is_short_circuiting(cur_stat_q = this->check(q, trace_pt))){
        return statistic(cur_stat_q,ret_stat);
      }
      else if ((cur_stat_p = this->check(p, trace_pt)).is_satisfied){
        return statistic(statistic(cur_stat_q, cur_stat_p), ret_stat);
      } else {
        ret_stat = statistic(cur_stat_q, ret_stat);
        trace_pt++;
      }
    }
    
    return statistic(ret_stat, statistic(false, 0, 1)); 

}

/**
 * p W q will be true if !p does not occur before (exclusive) q
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::weakuntil_check(const ltl::binop* node,
        const event* trace_pt, std::set<int> trace_ids) {
    const ltl::formula * p = node->first();
    const ltl::formula * q = node->second();

    statistic cur_stat_p;
    statistic cur_stat_q;
    statistic ret_stat = statistic(true, 0, 0);
    
    while (!trace_pt->is_terminal()){
      if ((cur_stat_q = this->check(q, trace_pt)).is_satisfied){
        return statistic(statistic(true, 0, 0),ret_stat);
      }
      else if (is_short_circuiting(cur_stat_p = this->check(p, trace_pt))) {
        return statistic(cur_stat_p,ret_stat);
      } else {
        ret_stat = statistic(cur_stat_p, ret_stat);
        trace_pt++;
      }

    }
    return statistic(ret_stat, statistic(true, 0, 0)); 


}


/**
 * Gp will be true if Gp is true on every trace suffix of the given
 
 * trace, i.e. if p holds at all time
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::globally_check(const ltl::unop* node,
        const event* trace_pt, std::set<int> trace_ids) {
    const ltl::formula * p = node->child();

    statistic cur_stat;
    statistic ret_stat = statistic(true,0,0);
    
    while (!trace_pt->is_terminal()){
      if (is_short_circuiting (cur_stat = this->check(p, trace_pt))){
        return statistic(cur_stat, ret_stat);
      }
      else {
        ret_stat = statistic(cur_stat, ret_stat);
      }
      trace_pt++;
    }
    
    return ret_stat;
    
}


/**
 * Fp will be true if Fp is true on some trace suffix of the given
 * trace, i.e. if p holds at some time
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::finally_check(const ltl::unop* node,
        const event* trace_pt, std::set<int> trace_ids) {
    const ltl::formula * p = node->child();

    // TODO: why not combine the support, support pot here? (like in globally)
    statistic cur_stat;
    while (!trace_pt->is_terminal()){
      if ((cur_stat = this->check(p,trace_pt)).is_satisfied){
        return cur_stat;
      }
      else {
        trace_pt++;
      }      
    }
    
    return statistic(false,0,1);
}


/**
 * Xp will be true if p is true on the next suffix of the given
 * trace, i.e. if p holds at the next time
 * @param node: the atomic proposition to check
 * @param trace: pointer to the start of the trace
 * @param trace_ids
 * @return
 */
statistic linear_trace_checker::next_check(const ltl::unop* node,
        const event* trace_pt, std::set<int> trace_ids) {
    const ltl::formula * p = node->child();
    // if we are at the terminal event, the next event is also a terminal
    // event. Since we are traversing a finite tree, this will terminate.
    if (trace_pt->is_terminal()) {
        return this->check(p, trace_pt);
    }
    return this->check(p, trace_pt + 1);
}

/**
 * Finds valid instants on a given set of traces using a linear checker
 * configured to the default (i.e. "vanilla") setting.
 * @param prop_type property type to check.
 * @param instantiator instantiator to produce next instantiation
 * @param traces all the traces to check on
 * @return
 */
vector<std::pair<map<string, string>, statistic>> valid_instants_on_traces(
        const ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<std::multiset<vector<event>>> traces,
        bool use_invar_semantics,
        shared_ptr<map<string,string>> translations) {
    return valid_instants_on_traces(prop_type, instantiator, traces, settings(), use_invar_semantics, translations);
}

/**
 * Using custom configuration of the linear checker, check all
 * instantiations of the property type given on the traces and return
 * the ones which are valid (according to the specified checker settings).
 * @param prop_type property type to check.
 * @param instantiator instantiator to produce next instantiation
 * @param traces all the traces to check on
 * @param c_settings user-specified configuration for checker
 * @return
 */
vector<std::pair<map<string, string>, statistic>> valid_instants_on_traces(
        const ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<std::multiset<vector<event>>> traces,
        settings c_settings,
        bool use_invar_semantics,
        shared_ptr<map<string,string>> translations) {
    instantiator->reset_instantiations();
    // vector to return
    vector<std::pair<map<string, string>, statistic>> return_vec;
    if (use_invar_semantics && (translations == NULL)){
        std::cerr << "Wanted to use invar semantics but no translations were provided.\n";
        return return_vec;
    }
    linear_trace_checker checker(use_invar_semantics,translations);
    // set checker configurations
    checker.configure(c_settings);
    // simplifier for turning formulas into negative normal form so that
    // statistics of formulae involving not, xor, <-> can be computed.
    std::unique_ptr<ltl::ltl_simplifier> simplifier(new ltl::ltl_simplifier());

    // Loop through each instantiation, filtering out those which are invalid (the notion of invalidity
    // depending on the given checker settings).
    // Note that it would be nice to output the number of instantiations which were found to be true
    // on the trace set but failed to satisfy given statistical thresholds and were subsequently filtered out.
    // This however would require certain short-circuiting to be turned off, and so the current implementation
    // does not support this feature.
    while (true) {
        shared_ptr<map<string,string>> current_instantiation = instantiator->get_next_instantiation();
        if (current_instantiation == NULL) {
            break;
        }
        const ltl::formula * instantiated_prop_type = instantiate(prop_type,*current_instantiation,
                instantiator->get_events_to_exclude());
        // unless checker is configured for the vanilla setting, turn formula into negative normal form.
        // note that the the operators xor, <->, and -> will be reduced to basic operators by the below code.
        if (!c_settings.is_vanilla()) {
            // move original formula to a temp ptr to be destroyed
            const ltl::formula * to_delete = instantiated_prop_type;
            instantiated_prop_type = simplifier->negative_normal_form(instantiated_prop_type);
            to_delete->destroy();
        }

        // the meaning of validity will depend on the user inputed settings
        bool valid = true;
        statistic global_stat = statistic(true, 0, 0);
        for (auto each = traces->begin(); each != traces->end(); each=traces->upper_bound(*each)) {
            statistic trace_stat = checker.check_on_trace(instantiated_prop_type,&(each->at(0)));
            // when there are multiple equivalent traces, the instantiation is only checked over one
            // of them. To compute the correct statistics, multiply the statistics for each trace by
            // the number of "occurrences" of said trace in the log.
            int trace_count = traces->count(*each);
            trace_stat.support = trace_stat.support * trace_count;
            trace_stat.support_potential = trace_stat.support_potential * trace_count;
            global_stat = statistic(global_stat, trace_stat);

            // if confidence threshold is 1.0, an instantiation becomes invalid on the first unsatisfied trace,
            // so we short-circuit after flagging the current instantiation as invalid.
            if (c_settings.conf_t == 1.0 && !trace_stat.is_satisfied) {
                valid = false;
                break;
            }
            // in non-global setting, an instantiation becomes invalid on the first trace where a threshold is
            // unsatisfied, so we short-circuit after flagging the current instantiation as invalid.
            if (!c_settings.use_global_t && !c_settings.satisfies_thresholds(trace_stat)) {
                valid = false;
                break;
            }
            // in global 0-conf threshold setting, an instantiation becomes valid on the full trace set once
            // every threshold have been satisfied, so we short-circuit after flagging the current instantiation as valid.
            if (c_settings.use_global_t && c_settings.conf_t == 0.0) {
                if (!c_settings.compute_full_stats && c_settings.satisfies_thresholds(global_stat)) {
                    valid = true;
                    break;
                }
            }
        }
        // in the global setting, we need to check that the final statistics (i.e. the sum of statistics computed
        // at each trace) satisfy the given thresholds.
        if (c_settings.use_global_t) {
            valid = c_settings.satisfies_thresholds(global_stat);
        }

        instantiated_prop_type->destroy();
        // return finding if it is valid and its statistics meets all specified thresholds
        if (valid) {
            std::pair<map<string, string>, statistic> finding(*current_instantiation, global_stat);
            return_vec.push_back(finding);
        }
    }
    return return_vec;
}

}
/* namespace texada */
