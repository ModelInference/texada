/*
 * pregeninstantspool.h
 *
 *  Created on: May 21, 2014
 *      Author: clemieux
 */

#ifndef PREGENINSTANTSPOOL_H_
#define PREGENINSTANTSPOOL_H_

#include <set>
#include <string>
#include <map>
#include <ltlast/allnodes.hh>
#include <ltlvisit/apcollect.hh>
#include <memory>

#include "instantspoolcreator.h"

namespace texada {
using std::shared_ptr;
using std::vector;
using std::string;
using std::map;
using std::set;


/**
 * class which creates a pool of instantiating functions (i.e. maps) given
 * the number of variables in an LTL formula and a set of events from traces.
 * Stores the pool as a vector.
 */
class pregen_instants_pool : public instants_pool_creator{
public:

    // describes a single instantiation function,
    // this maintains a vector of these
    struct inst_fxn {
        // formula variables -> event variables
        map<string, string> inst_map;
        bool valid = true;
    };

    pregen_instants_pool(shared_ptr<set<string>> events, spot::ltl::atomic_prop_set *, bool allow_reps);
    virtual ~pregen_instants_pool();
    void reset_insantiations();
    shared_ptr<map<string,string>> get_next_instantiation();
    shared_ptr<vector<inst_fxn>> return_instantiations();

private:
    // the full set of instantiations, to
    // be returned
    void instantiate_array();
    void traverse_and_fill(string event, int i, int k);
    shared_ptr<vector<inst_fxn>> inst_pool;
    // to return the instantiations in correct order.


};

} /* namespace texada */

#endif /* PREGENINSTANTSPOOL_H_ */
