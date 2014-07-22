/*
 * truncatingchecker.cpp
 *
 *  Created on: Jul 7, 2014
 *      Author: clemieux
 */

#include "truncatingchecker.h"
#include <ltlvisit/apcollect.hh>
#include <ltlvisit/tostring.hh>
#include "../instantiation-tools/apsubbingcloner.h"

namespace texada {

truncating_checker::truncating_checker(const spot::ltl::formula* node,
        shared_ptr<set<string>> events_) :
        events(events_) {
    set_up_iteration_tracker(node);

}

truncating_checker::~truncating_checker() {
    // TODO Auto-generated destructor stub
}
/**
 * Sets up the "iteration tracker" for a given formula and stored events;
 * this aids us in knowing which instantiation fxn should next be created
 * given our "position" when we create/check instantiations later on.
 * @param node
 */
void truncating_checker::set_up_iteration_tracker(
        const spot::ltl::formula* node) {
    spot::ltl::atomic_prop_set * form_vars = spot::ltl::atomic_prop_collect(
            node);
    int form_vars_size = form_vars->size();
    // setting to the iterator position just before the event will simplify the
    // switching of event iterator at position 0
    set<string>::iterator events_it = events->end();
    events_it--;

    // for each of the formula variables, we add an entry to the to the
    // iterator helper containing the formula variable, how often to switch
    // it,  and the second-to-last iterator.
    // e.g. with formula vars x,y,z and events a,b,c,d, we'll set up:
    // x, 4^(3-1-0)=16, it to d
    // y, 4^(3-1-1)=4, it to d
    // z, 4^(3-1-2)=1, it to d;
    // so when we start to fill we'll see..
    // |x->a|x->a|x->a|x->a|x->a|x->a|x->a|x->a|x->a|x->a|x->a|x->a|x->a|x->a|x->a|x->a|...
    // |y->a|y->a|y->a|y->a|y->b|y->b|y->b|y->b|y->c|y->c|y->c|y->c|y->d|y->d|y->d|y->d|...
    // |z->a|z->b|z->c|z->d|z->a|z->b|z->c|z->d|z->a|z->b|z->c|z->d|z->a|z->b|z->c|z->d|...
    // as the order in which the mappings are created.
    spot::ltl::atomic_prop_set::iterator form_vars_it = form_vars->begin();
    for (int i = 0; i < form_vars_size; i++) {
        iter_store insert;
        insert.mapto = events_it;
        const spot::ltl::atomic_prop * var = *form_vars_it;
        insert.mapfrom = var->name();
        insert.switchvar = pow(events->size(), form_vars_size - 1 - i);
        iteration_tracker.push_back(insert);
        form_vars_it++;
    }
    delete form_vars;

}

/**
 * Creates a vector containing all valid instantiation functions. Dynamically creates
 * instantiations, and rejects them or checks them according to how the position
 * @return
 */
vector<map<string, string>> truncating_checker::return_valid_instants(
        const spot::ltl::formula* node, set<vector<string_event>> traces) {
    vector<map<string, string>> return_vec;
    map<string,string> inst_map_at_pos;
    unsigned long total_size = pow(events->size(), iteration_tracker.size());
    for (unsigned long pos = 0; pos < total_size; pos++) {
        // erase the map from last time
        inst_map_at_pos.clear();
        //update the iteration tracker for the current position. On 1st entry,
        // the line it->mapto++; should push it->mapto to events.end(),
        // thus correctly starting at events.begin()
        for (vector<iter_store>::iterator it = iteration_tracker.begin();
                it != iteration_tracker.end(); it++) {
            if (pos % it->switchvar == 0) {
                it->mapto++;
                if (it->mapto == events->end())
                    it->mapto = events->begin();
            }
            inst_map_at_pos.emplace(it->mapfrom,*(it->mapto));
            //std::cout << "Emplaced " << it->mapfrom << "->" << *(it->mapto) << "\n";
        }


        //############TODO: this is just a 'stub' implementation for now.
        //instantiate the formula with the current instantiation function
        const spot::ltl::formula* instantiated_form = instantiate(node,
                inst_map_at_pos);
        //std::cout << spot::ltl::to_string(instantiated_form) << "\n";
        //create a checker, set validity of formula to true
        linear_trace_checker checker = linear_trace_checker();
        bool is_valid = true;

        // check formula on each trace in the trace set; break
        // once it does not hold on one trace
        for (set<vector<string_event>>::iterator trace_it = traces.begin();
                trace_it != traces.end(); trace_it++){
            if (!checker.check(instantiated_form,&(*trace_it)[0])){
                is_valid = false;
                break;
            }
        } // end of inner for

        if (is_valid){
            return_vec.push_back(inst_map_at_pos);
        }
        instantiated_form->destroy();
        //############end of stub

        //std::cout << "---- \n";
    } // end of outer for

    return return_vec;
}

/**
 * Return true if the instantiating function corresponding to pos will be false;
 * false if it needs to be checked.
 * @param pos
 * @return
 */
bool truncating_checker::meets_trunc_criteria(unsigned long pos) {
    // stub
    return false;
}

} /* namespace texada */
