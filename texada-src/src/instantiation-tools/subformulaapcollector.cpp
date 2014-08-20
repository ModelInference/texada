/*
 * subformulaapcollector.cpp
 *
 *  Created on: Aug 19, 2014
 *      Author: clemieux
 */

#include "subformulaapcollector.h"
#include <ltlvisit/apcollect.hh>

namespace texada {

subformula_ap_collector::subformula_ap_collector() {
    // TODO Auto-generated constructor stub

}

subformula_ap_collector::~subformula_ap_collector() {
    // TODO Auto-generated destructor stub
}

void subformula_ap_collector::doit(const spot::ltl::atomic_prop* ap) {

}
void subformula_ap_collector::doit(const spot::ltl::constant* c) {

}
void subformula_ap_collector::doit_default(const spot::ltl::formula* f) {
    spot::ltl::atomic_prop_set * atomic_props = spot::ltl::atomic_prop_collect(
            f);
    std::set<std::string> ap_names;
    for (spot::ltl::atomic_prop_set::iterator it = atomic_props->begin();
            it != atomic_props->end(); it++) {
        ap_names.insert((*it)->name());
    }
    delete atomic_props;
    subform_ap_set.emplace(f,ap_names);


}

} /* namespace texada */
