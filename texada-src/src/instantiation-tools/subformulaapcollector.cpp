/*
 * subformulaapcollector.cpp
 *
 *  Created on: Aug 19, 2014
 *      Author: clemieux
 */

#include "subformulaapcollector.h"
#include "texadatospotmapping.h"
#include <iostream>

namespace texada {

subformula_ap_collector::subformula_ap_collector() {
    // TODO Auto-generated constructor stub

}

subformula_ap_collector::~subformula_ap_collector() {
    // TODO Auto-generated destructor stub
}

void subformula_ap_collector::doit(const ltl::atomic_prop* ap) {
}

void subformula_ap_collector::doit(const ltl::constant* c) {

}
void subformula_ap_collector::doit_default(const ltl::formula* f) {
    ltl::atomic_prop_set * atomic_props = ltl::atomic_prop_collect(f);
    std::set<std::string> ap_names;
    for (ltl::atomic_prop_set::iterator it = atomic_props->begin();
            it != atomic_props->end(); it++) {
        ap_names.insert((*it)->name());
    }
    delete atomic_props;
    subform_ap_set.emplace(f,ap_names);


}

} /* namespace texada */
