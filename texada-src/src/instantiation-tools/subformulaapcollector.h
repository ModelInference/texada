/*
 * subformulaapcollector.h
 *
 *  Created on: Aug 19, 2014
 *      Author: clemieux
 */

#ifndef SUBFORMULAAPCOLLECTOR_H_
#define SUBFORMULAAPCOLLECTOR_H_

#include "texadatospotmapping.h"
#include "texadapostfix.h"
#include <map>
#include <set>
#include <string>

namespace texada {

/**
 * Collects the atomic propositions of all the subformulae of a given ltl formula.
 * Stores these atomic propositions in a subformula -> atomic proposition name map.
 * Also collects atomic propositions for the input formula in negative normal form
 * and the negation of the input formula in negative normal form and a map
 * of p -> neno(!p).
 */
class subformula_ap_collector : public ltl::postfix_visitor{
public:
    subformula_ap_collector();
    virtual ~subformula_ap_collector();

    std::map<const ltl::formula*,std::set<std::string>> subform_ap_set;

    virtual void doit(const ltl::atomic_prop* ap);
    virtual void doit(const ltl::constant* c);
    virtual void doit_default(const ltl::formula* f);

};

} /* namespace texada */

#endif /* SUBFORMULAAPCOLLECTOR_H_ */
