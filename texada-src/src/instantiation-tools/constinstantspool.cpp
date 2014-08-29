/*
 * constinstantspool.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: dennis
 */

#include "constinstantspool.h"
#include "vector"

namespace texada {

const_instants_pool::const_instants_pool(const spot::ltl::formula * formula) {
    instantiation = std::make_shared<map<string, string>>();
    instantiate(formula);
}

const_instants_pool::~const_instants_pool() {
    // TODO Auto-generated destructor stub
}

shared_ptr<map<string, string>> const_instants_pool::get_next_instantiation() {
    if (traversal_var > 0) {
        return NULL;
    }
    traversal_var++;
    return instantiation;
}

void const_instants_pool::instantiate(const spot::ltl::formula * formula) {
    shared_ptr<spot::ltl::atomic_prop_set> variables(
            spot::ltl::atomic_prop_collect(formula));
    for (spot::ltl::atomic_prop_set::iterator it = variables->begin();
            it != variables->end(); it++) {
        std::pair<string, string> mapping ((*it)->name(), (*it)->name());
        instantiation->insert(mapping);
    }
}

} /* namespace texada */
