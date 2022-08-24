/*
 * otfinstantspool.cpp
 *
 *  Created on: Jul 7, 2014
 *      Author: clemieux
 */

#include "otfinstantspool.h"
#include <ltlvisit/apcollect.hh>
#include <ltlvisit/tostring.hh>
#include "apsubbingcloner.h"
#include <math.h>

namespace texada {

otf_instants_pool::otf_instants_pool(shared_ptr<set<string>> events,
        shared_ptr<spot::ltl::atomic_prop_set> ltlevents, bool allow_reps,
        vector<string> exclude_events) :
        instants_pool_creator(events, ltlevents, allow_reps, exclude_events) {
    set_up_iteration_tracker();

}

otf_instants_pool::~otf_instants_pool() {
    // TODO Auto-generated destructor stub
}
/**
 * Sets up the "iteration tracker" for a given formula and stored events;
 * this aids us in knowing which instantiation fxn should next be created
 * given our "position" when we create/check instantiations later on.
 * @param node
 */
void otf_instants_pool::set_up_iteration_tracker() {

    int form_vars_size = formula_vars->size();

    // setting to the iterator position just before the event will simplify the
    // switching of event iterator at position 0

    set<string>::iterator events_it = unique_events->end();

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
    spot::ltl::atomic_prop_set::iterator form_vars_it = formula_vars->begin();
    for (int i = 0; i < form_vars_size; i++) {
        iter_store insert;
        insert.mapto = events_it;
        const spot::ltl::atomic_prop * var = *form_vars_it;
        insert.mapfrom = var->name();
        insert.switchvar = pow(unique_events->size(), form_vars_size - 1 - i);
        iteration_tracker.push_back(insert);
        form_vars_it++;
    }

}

/**
Returns true if the provided map has any duplicate bindings.
*/
bool map_has_duplicates(shared_ptr<map<string, string>> inst_map_at_pos){
    set<string> found_so_far;        
    for (map<string, string>::iterator map_it = inst_map_at_pos->begin();
        map_it != inst_map_at_pos->end(); map_it++) {
        if (found_so_far.find(map_it->second) == found_so_far.end()) {
            found_so_far.insert(map_it->second);
        } else {
            return true;
        }
    }
    return false;
}

/**
 * Creates a vector containing all valid instantiation functions. Dynamically creates
 * instantiations, and rejects them or checks them according to how the position
 * @return
 */
shared_ptr<map<string, string>> otf_instants_pool::get_next_instantiation() {
    shared_ptr<map<string, string>> inst_map_at_pos = std::make_shared<
        map<string, string>>();
    do {
        inst_map_at_pos->clear();
        if (traversal_var >= pow(unique_events->size(), iteration_tracker.size())) {
            return NULL;
        }
        //update the iteration tracker for the current position. On 1st entry,
        // the line it->mapto++; should push it->mapto to events.end(),
        // thus correctly starting at events.begin()
        for (vector<iter_store>::iterator it = iteration_tracker.begin();
                it != iteration_tracker.end(); it++) {
            if (traversal_var % it->switchvar == 0) {
                it->mapto++;
                if (it->mapto == unique_events->end())
                    it->mapto = unique_events->begin();
            }
            inst_map_at_pos->emplace(it->mapfrom, *(it->mapto));
        }
        traversal_var++;
    }
    /* If this map has repetitions and that's not allowed, skip to the next instantiation. */
    while (!allow_repetition && map_has_duplicates(inst_map_at_pos));
    
    return inst_map_at_pos;

}

} /* namespace texada */
