/*
 * instantspoolcreator.h
 *
 *  Created on: Jul 21, 2014
 *      Author: clemieux
 */

#ifndef INSTANTSPOOLCREATOR_H_
#define INSTANTSPOOLCREATOR_H_

#include <memory>
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
class instants_pool_creator {

public:
    instants_pool_creator(shared_ptr<set<string>>  events,
            spot::ltl::atomic_prop_set * ltlevents, bool allow_reps);
    virtual ~instants_pool_creator();
    virtual map<string,string> get_next_instantiation();
protected:
    shared_ptr<set<string>>  unique_events;
    spot::ltl::atomic_prop_set * formula_vars;
    bool allow_repetition = false;


};

} /* namespace texada */

#endif /* INSTANTSPOOLCREATOR_H_ */
