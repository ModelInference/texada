/*
 * scopedinstantspoolcreator.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: clemieux
 */

#include "scopedinstantspoolcreator.h"
#include <math.h>

namespace texada {

scoped_instants_pool_creator::scoped_instants_pool_creator(shared_ptr<set<string>> events,
        shared_ptr<spot::ltl::atomic_prop_set> ltlevents, bool allow_reps,
        vector<string> exclude_events) :
        instants_pool_creator(events, ltlevents, allow_reps, exclude_events)  {
    scope_events = new set<string>();
    predicate_events = new set<string>();
    for (set<string>::iterator it = events->begin(); it != events->end(); it++){
        if (it->find("!!!") == 0){
           // std::cout << *it << " is a scope event\n";
           scope_events->insert(*it);
        } else if (it->find("???") == 0){
           // std::cout << *it << " is a predicate event\n";
           predicate_events->insert(*it);
        }
    }
    std::cout << "Number of predicates: " << predicate_events->size() << "\n";
    scope_aps = new set<const spot::ltl::atomic_prop *>();
    predicate_aps = new set<const spot::ltl::atomic_prop *>();
    for (spot::ltl::atomic_prop_set::iterator it = formula_vars->begin(); it != formula_vars->end(); it++){
        const spot::ltl::atomic_prop * var = *it;
        string varname = var->name();
        if (varname.find("scope_") == 0){
            //std::cout << varname << " is a scope ap\n";
            scope_aps->insert(var);
        } else {
           // std::cout << varname << " is a predicate ap\n";
            predicate_aps->insert(var);
        }
    }

    set_up_iteration_tracker();


}

/**
 * Sets up the "iteration tracker" for a given formula and stored events;
 * this aids us in knowing which instantiation fxn should next be created
 * given our "position" when we create/check instantiations later on.
 * @param node
 */
void scoped_instants_pool_creator::set_up_iteration_tracker() {
    int predicate_vars_size = predicate_aps->size();

    // setting to the iterator position just before the event will simplify the
    // switching of event iterator at position 0

    set<string>::iterator events_it = predicate_events->end();

    --events_it;

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
    set<const spot::ltl::atomic_prop *>::iterator predicate_vars_it = predicate_aps->begin();
    for (int i = 0; i < predicate_vars_size; i++) {
        iter_store insert;
        insert.mapto = events_it;
        const spot::ltl::atomic_prop * var = *predicate_vars_it;
        insert.mapfrom = var->name();
        insert.switchvar = pow(predicate_events->size(), predicate_vars_size - 1 - i);
        iteration_tracker.push_back(insert);
        predicate_vars_it++;
    }

}


scoped_instants_pool_creator::~scoped_instants_pool_creator() {
    delete scope_events;
    delete predicate_events;
    delete scope_aps;
    delete predicate_aps;

}

shared_ptr<map<string,string>>  scoped_instants_pool_creator::get_next_instantiation() {
    shared_ptr<map<string, string>> inst_map_at_pos = std::make_shared<
            map<string, string>>();

    if (traversal_var >= pow(predicate_events->size(), iteration_tracker.size())) {
        return NULL;
    }
    //update the iteration tracker for the current position. On 1st entry,
    // the line it->mapto++; should push it->mapto to events.end(),
    // thus correctly starting at events.begin()
    for (vector<iter_store>::iterator it = iteration_tracker.begin();
            it != iteration_tracker.end(); it++) {
        if (traversal_var % it->switchvar == 0) {
            it->mapto++;
            if (it->mapto == predicate_events->end())
                it->mapto = predicate_events->begin();
        }
        inst_map_at_pos->emplace(it->mapfrom, *(it->mapto));
        string full_map_to_string = *(it->mapto);
        std::size_t first_split = full_map_to_string.find("???");
        std::size_t second_split = full_map_to_string.find("???", first_split + 1);
        string to_place = "!!!" + full_map_to_string.substr(first_split + 3,second_split -3 );
        inst_map_at_pos->emplace("scope_"+ it->mapfrom, to_place);
    }

    // if we don't allow repetition, check that this map is valid
    if (!allow_repetition) {
        set<string> found_so_far;
        for (map<string, string>::iterator map_it = inst_map_at_pos->begin();
                map_it != inst_map_at_pos->end(); map_it++) {
            // add any unknown bindings to what was found so far
            if (found_so_far.find(map_it->second) == found_so_far.end()) {
                found_so_far.insert(map_it->second);
            }
            // if the binding we re-found was scope, is ok. Only skip
            // this instantiation if it's not a scope variable.
            else if (map_it->second.find("!!!") != 0){
                traversal_var++;
                return get_next_instantiation();
            }

        }
    }

    // end
    traversal_var++;
    return inst_map_at_pos;

}

} /* namespace texada */
