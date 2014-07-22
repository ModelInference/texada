/*
 * instantspoolcreator.cpp
 *
 *  Created on: Jul 21, 2014
 *      Author: clemieux
 */

#include "instantspoolcreator.h"

namespace texada {

instants_pool_creator::instants_pool_creator(shared_ptr<set<string>> events_,
        spot::ltl::atomic_prop_set * ltlevents, bool allow_reps) :
        formula_vars(ltlevents), unique_events(events_), allow_repetition(allow_reps)  {

}

instants_pool_creator::~instants_pool_creator() {
    // TODO Auto-generated destructor stub
}

map<string,string> instants_pool_creator::get_next_instantiation(){
    return map<string,string>();
}

} /* namespace texada */
