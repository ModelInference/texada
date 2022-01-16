/*
 * scopedinstantspoolcreator.h
 *
 *  Created on: Apr 4, 2016
 *      Author: clemieux
 */

#ifndef TEXADA_SRC_SRC_INSTANTIATION_TOOLS_SCOPEDINSTANTSPOOLCREATOR_H_
#define TEXADA_SRC_SRC_INSTANTIATION_TOOLS_SCOPEDINSTANTSPOOLCREATOR_H_

#include "instantspoolcreator.h"
#include "texadatospotmapping.h"


namespace texada {

class scoped_instants_pool_creator: public instants_pool_creator {
public:
    scoped_instants_pool_creator(shared_ptr<set<string>> events,
            shared_ptr<ltl::atomic_prop_set> ltlevents, bool allow_reps,
            vector<string> exclude_events);
    virtual ~scoped_instants_pool_creator();
    shared_ptr<map<string,string>>  get_next_instantiation();
private:
    set<string> * scope_events;
    set<string> * predicate_events;
    set<const ltl::atomic_prop *> * scope_aps;
    set<const ltl::atomic_prop *> * predicate_aps;
    // structure to help create instantiations on the fly,
    // to allow a single pass through.
    struct iter_store{
        // at which points do we need to switch the
        // mapto variable
        int switchvar;
        // formula variable which will be substituted
        string mapfrom;
        // current position in the set of substituting
        // variables
        set<string>::iterator mapto;
    };

    // keeps track of each "level" we're adding to
    // the instantiation map
    vector<iter_store> iteration_tracker;

    void set_up_iteration_tracker();
};

} /* namespace texada */

#endif /* TEXADA_SRC_SRC_INSTANTIATION_TOOLS_SCOPEDINSTANTSPOOLCREATOR_H_ */
