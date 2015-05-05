/*
 * mostoccurringsubsetgatherer.cpp
 *
 *  Created on: May 5, 2015
 *      Author: clemieux
 */

#include "mostoccurringsubsetgatherer.h"

#include <ltlvisit/apcollect.hh>
namespace texada {

most_occurring_subset_gatherer::most_occurring_subset_gatherer() {
    // TODO Auto-generated constructor stub

}

most_occurring_subset_gatherer::~most_occurring_subset_gatherer() {
    // TODO Auto-generated destructor stub
}


void most_occurring_subset_gatherer::doit_default(const spot::ltl::formula* f){
    spot::ltl::atomic_prop_set * atomic_props = spot::ltl::atomic_prop_collect(
            f);
    if (atomic_props->size() > 1){
      std::set<std::string> ap_names;
      for (spot::ltl::atomic_prop_set::iterator it = atomic_props->begin();
              it != atomic_props->end(); it++) {
          ap_names.insert((*it)->name());
      }
      delete atomic_props;
      if (set_to_count_map.find(ap_names) != set_to_count_map.end()){
          set_to_count_map.at(ap_names) = set_to_count_map.at(ap_names) + 1;
      } else{
          set_to_count_map.emplace(ap_names,1);
      }

    }
}

} /* namespace texada */
