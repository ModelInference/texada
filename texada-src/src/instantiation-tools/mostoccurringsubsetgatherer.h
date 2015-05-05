/*
 * mostoccurringsubsetgatherer.h
 *
 *  Created on: May 5, 2015
 *      Author: clemieux
 */

#ifndef MOSTOCCURRINGSUBSETGATHERER_H_
#define MOSTOCCURRINGSUBSETGATHERER_H_

#include <ltlvisit/postfix.hh>
#include <set>
#include <string>
#include <map>

namespace texada {


/**
 * gathers the most occurring subset of atomic propositions
 * for use in determining instantiation order.
 *
 */
class most_occurring_subset_gatherer: public spot::ltl::postfix_visitor {
public:
    most_occurring_subset_gatherer();
    virtual ~most_occurring_subset_gatherer();
    virtual void doit_default(const spot::ltl::formula* f);
    std::map<std::set<std::string>, int> set_to_count_map;


};

} /* namespace texada */

#endif /* MOSTOCCURRINGSUBSETGATHERER_H_ */
