/*
 * instantspoolcreator.h
 *
 *  Created on: May 21, 2014
 *      Author: clemieux
 */

#ifndef INSTANTSPOOLCREATOR_H_
#define INSTANTSPOOLCREATOR_H_

#include <set>
#include <string>
#include <map>
#include <ltlast/allnodes.hh>
#include <ltlvisit/apcollect.hh>
#include <memory>

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
class instants_pool_creator {
public:

    // describes a single instantiation function,
    // this maintains a vector of these
    struct inst_fxn {
        // formula variables -> event variables
        map<string, string> inst_map;
        bool valid = true;
    };

    instants_pool_creator(set<string>& events, spot::ltl::atomic_prop_set);
    virtual ~instants_pool_creator();
    void instantiate_array();
    void traverse_and_fill(string event, int i, int k);
    shared_ptr<vector<inst_fxn>> return_instantiations() const;

private:
    // the full set of instantiations, to
    // be returned
    shared_ptr<vector<inst_fxn>> inst_pool;
    spot::ltl::atomic_prop_set formula_vars;
    set<string> * unique_events;
    int f_var_size;

};

} /* namespace texada */

#endif /* INSTANTSPOOLCREATOR_H_ */
