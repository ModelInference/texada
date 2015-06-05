/*
 * subformulaapcollector.cpp
 *
 *  Created on: Aug 19, 2014
 *      Author: clemieux
 */

#include "subformulaapcollector.h"
#include <ltlvisit/apcollect.hh>
#include <iostream>
#include <ltlvisit/nenoform.hh>

namespace texada {

subformula_ap_collector::subformula_ap_collector() {
    // TODO Auto-generated constructor stub

}

subformula_ap_collector::~subformula_ap_collector() {
    std::map<const spot::ltl::formula*,std::pair<std::set<std::string>,bool>>::iterator it = subform_ap_set.begin();
    while (it != subform_ap_set.end()){
       // std::cout << "Did we get here too?\n";
        std::pair<std::set<std::string>, bool> pair = it->second;
        if (pair.second){
         std::map<const spot::ltl::formula*,std::pair<std::set<std::string>,bool>>::iterator to_delete = it;
         it++;
         to_delete->first->destroy();
        }
        else {
            it++;
        }
    }
    subform_ap_set.clear();
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
    subform_ap_set.emplace(f,std::pair<std::set<std::string>,bool>(ap_names, 0));
    subform_ap_set.emplace(spot::ltl::negative_normal_form(f,false), std::pair<std::set<std::string>,bool>(ap_names, 1));
    subform_ap_set.emplace(spot::ltl::negative_normal_form(f,true), std::pair<std::set<std::string>,bool>(ap_names, 1));


}

} /* namespace texada */
