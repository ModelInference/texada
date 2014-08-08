/*
 * instantspoolcreator.h
 *
 *  Created on: Jul 21, 2014
 *      Author: clemieux
 */

#ifndef INSTANTSPOOLCREATOR_H_
#define INSTANTSPOOLCREATOR_H_

#include <memory>
#include <vector>
#include <map>
#include <set>
#include <string>

#include <ltlast/allnodes.hh>
#include <ltlvisit/apcollect.hh>

namespace texada {
using std::shared_ptr;
using std::map;
using std::string;
using std::set;
using std::vector;
/**
 * Parent class for instantiation pool creators
 */
class instants_pool_creator {

public:
    instants_pool_creator(shared_ptr<set<string>>  events,
            shared_ptr<spot::ltl::atomic_prop_set> ltlevents, bool allow_reps,
            vector<string> exclude_events = vector<string>());
    virtual ~instants_pool_creator();
    //return to the start of the instantiations.
    virtual shared_ptr<map<string,string>>  get_next_instantiation() = 0;
    virtual void reset_instantiations();
    virtual vector<string> get_events_to_exclude();
protected:
    shared_ptr<set<string>>  unique_events;
    vector<string> events_to_exclude;
    shared_ptr<spot::ltl::atomic_prop_set> formula_vars;
    bool allow_repetition = false;
    int traversal_var = 0;


};

} /* namespace texada */

#endif /* INSTANTSPOOLCREATOR_H_ */
