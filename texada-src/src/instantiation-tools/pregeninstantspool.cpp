/*
 * pregeninstantspool.cpp
 *
 *  Created on: May 21, 2014
 *      Author: clemieux
 */

#include "pregeninstantspool.h"
#include <math.h>
#include "orderdecider.h"

namespace texada {

/**
 * Create instantiator, and fill the array of instantiations
 * @param events events in all traces inputted
 * @param ltlevents set of atomic propositions to be replaced
 */
pregen_instants_pool::pregen_instants_pool(shared_ptr<set<string>> events_,
        const spot::ltl::formula * f, bool allow_reps, bool opt_order,
        shared_ptr<spot::ltl::atomic_prop_set> set, vector<string> exclude_events) :
        instants_pool_creator(events_, f, allow_reps, opt_order, set, exclude_events) {
    // create formula vars vector
    vector<string> formula_vars;

    if (optimize_order) {
        formula_vars = optimize_var_order(formula, formula_aps->size());
    } else {
        // put into vector
        for (spot::ltl::atomic_prop_set::iterator it = formula_aps->begin();
                it != formula_aps->end(); it++){
            formula_vars.push_back((*it)->name());
        }
   }

    // We are creating a vector with the exact size required to store
    // all instantiations:
    // Each event can appear at each formula variable, so the total
    // size is: # of unique events ^ # of variable in formula
    inst_pool = std::make_shared<vector<map<string, string>>>(
    vector<map<string, string>>(pow(unique_events->size(), formula_vars.size())));
    instantiate_array(formula_vars);

}

pregen_instants_pool::~pregen_instants_pool() {
}
/**
 * Places all instantiations into the array.
 */
void pregen_instants_pool::instantiate_array(vector<string> formula_vars) {

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


    int f_var_size = formula_vars.size();
    for (int i = 0; i < f_var_size; i++) {
        // inst_fxn has a string->string map, so get name of atomic prop
        string name = formula_vars[i];
        // now traverse the array to fill this level
        traverse_and_fill(name, f_var_size -i -1, num_unique_events);

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
    for (vector<map<string, string>>::iterator inst_pool_it =
            inst_pool->begin(); inst_pool_it != inst_pool->end();
            inst_pool_it++) {
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
        inst_pool_it->insert(
                std::pair<string, string>(formula_event, *event_iterator));
        switch_var--;

    }
}

/**
 * Return all pre-generated instantiations at a time.
 * @return
 */
shared_ptr<vector<map<string, string>>> pregen_instants_pool::return_instantiations() {
    return inst_pool;
}

/**
 * Return the next instantiation.
 * @return
 */
shared_ptr<map<string, string>> pregen_instants_pool::get_next_instantiation() {
    if (traversal_var >= inst_pool->size()) {
        return NULL;
    }
    shared_ptr<map<string, string>> to_return = std::make_shared<
            map<string, string>>(inst_pool->at(traversal_var));
    traversal_var++;
    if (allow_repetition == false) {
        set<string> check_vars;
        for (map<string, string>::iterator map_it = to_return->begin();
                map_it != to_return->end(); map_it++) {
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

/**
 * Reset the instantiations so that the next call of
 * get_next_instantiation return the first instantiation.
 */
void pregen_instants_pool::reset_insantiations() {
    traversal_var = 0;
}

} /* namespace texada */
