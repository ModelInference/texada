/*
 * instantspoolcreator.cpp
 *
 *  Created on: Jul 21, 2014
 *      Author: clemieux
 */

#include "instantspoolcreator.h"
#include "vector"

namespace texada {

instants_pool_creator::instants_pool_creator(shared_ptr<set<string>> events_,
        const spot::ltl::formula * f, bool allow_reps, bool opt_order,
        vector<string> exclude_events) :
        formula(f), unique_events(events_), allow_repetition(allow_reps) ,
        optimize_order(opt_order), events_to_exclude(exclude_events){

}

instants_pool_creator::instants_pool_creator() {
    unique_events = NULL;
    formula = NULL;
    allow_repetition = false;
}

instants_pool_creator::~instants_pool_creator() {
}

void instants_pool_creator::reset_instantiations(){
    traversal_var = 0;
}

vector<string> instants_pool_creator::get_events_to_exclude(){
    return events_to_exclude;
}
} /* namespace texada */
