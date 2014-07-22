/*
 * pregeninstantspool.cpp
 *
 *  Created on: May 21, 2014
 *      Author: clemieux
 */

#include "pregeninstantspool.h"
#include <math.h>

namespace texada {

/**
 * Create instantiator, and fill the array of instantiations
 * @param events events in all traces inputted
 * @param ltlevents set of atomic propositions to be replaced
 */
pregen_instants_pool::pregen_instants_pool(shared_ptr<set<string>> events_,
        spot::ltl::atomic_prop_set * ltlevents, bool allow_reps) :
        instants_pool_creator(events_, ltlevents, allow_reps), traversal_var(0) {
    // We are creating a vector with the exact size required to store
    // all instantiations:
    // Each event can appear at each formula variable, so the total
    // size is: # of unique events ^ # of variable in formula

    inst_pool = std::make_shared<vector<inst_fxn>>(
            vector<inst_fxn>(pow(unique_events->size(), formula_vars->size())));

}

pregen_instants_pool::~pregen_instants_pool() {
}
/**
 * Places all instantiations into the array.
 */
void pregen_instants_pool::instantiate_array() {

    int num_unique_events = unique_events->size();

    // We must produce all permutations with replacement of length f_var_size
    // of the events. We go through the entire return array multiple times
    // in order to construct all permutations. Each pass adds a "level"
    // of mapping to each map. For the simple example with two bindings
    // x and y and three events a, b, c, the process goes like this:
    // INITIAL ARRAY:
    // |____|____|____|____|____|____|____|____|____|
    // FIRST PASS, level 0:
    // |x->a|x->b|x->c|x->a|x->b|x->c|x->a|x->b|x->c|
    // SECOND PASS, level `:
    // |x->a|x->b|x->c|x->a|x->b|x->c|x->a|x->b|x->c|
    // |y->a|y->a|y->a|y->b|y->b|y->b|y->c|y->c|y->c|
    // at this point, the iteration terminates.

    // "level" of the pass, i.e. how deep in formula_vars we are

    int lvl = 0;
    int f_var_size = formula_vars->size();
    for (set<const spot::ltl::atomic_prop*>::iterator formula_it =
            formula_vars->begin(); formula_it != formula_vars->end();
            ++formula_it) {
        assert(lvl < f_var_size);
        // inst_fxn has a string->string map, so get name of atomic prop
        const spot::ltl::atomic_prop* event_var = *formula_it;
        string name = event_var->name();
        // now traverse the array to fill this level
        traverse_and_fill(name, lvl, num_unique_events);
        lvl++;
    }
}
/**
 * Traverse and fill the inst_fxn array with the correct mapping, according
 * to how "deep" we are in iteration through the formula variables
 * @param formula_event Name of the event to be the first argument of map
 * @param i depth of iteration through formula variables.
 * @param k number of events
 */
void pregen_instants_pool::traverse_and_fill(string formula_event, int lvl,
        int num_unique_events) {
    //indicates when to continue iteration through events
    int switch_var = pow(num_unique_events, lvl);
    // find array size
    //begin with the first event
    set<string>::iterator event_iterator = unique_events->begin();
    for (vector<inst_fxn>::iterator inst_pool_it = inst_pool->begin();
            inst_pool_it != inst_pool->end(); inst_pool_it++) {
        // if we are at switching point, iterate to next event
        if (switch_var == 0) {
            ++event_iterator;
            // if we've finished passing through the events,
            // go back to the start
            if (event_iterator == unique_events->end())
                event_iterator = unique_events->begin();
            // and reset the switch to the top value
            switch_var = pow(num_unique_events, lvl);
        }
        inst_pool_it->inst_map.insert(
                std::pair<string, string>(formula_event, *event_iterator));
        switch_var--;

    }
}

/**
 * Return all pre-generated instantiations at a time.
 * @return
 */
shared_ptr<vector<pregen_instants_pool::inst_fxn>> pregen_instants_pool::return_instantiations() {
    instantiate_array();
    return inst_pool;
}

map<string, string> pregen_instants_pool::get_next_instantiation() {
    map<string, string> to_return = inst_pool->at(traversal_var).inst_map;
    traversal_var++;
    if (traversal_var >= inst_pool->size()) {
        traversal_var = 0;
    }
    if (allow_repetition == false) {
        set<string> check_vars;
        for (map<string, string>::iterator map_it = to_return.begin();
                map_it != to_return.end(); map_it++) {
            if (check_vars.find(map_it->second) == check_vars.end()) {
                check_vars.insert(map_it->second);
            } else {
                return get_next_instantiation();
            }
        }
        return to_return;
    } else {
        return to_return;

    }
}

} /* namespace texada */
