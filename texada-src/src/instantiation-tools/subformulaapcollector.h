/*
 * subformulaapcollector.h
 *
 *  Created on: Aug 19, 2014
 *      Author: clemieux
 */

#ifndef SUBFORMULAAPCOLLECTOR_H_
#define SUBFORMULAAPCOLLECTOR_H_

#include <ltlvisit/postfix.hh>
#include <map>
#include <set>
#include <string>

namespace texada {

class subformula_ap_collector : public spot::ltl::postfix_visitor{
public:
    subformula_ap_collector();
    virtual ~subformula_ap_collector();

    std::map<const spot::ltl::formula*,std::set<std::string>> subform_ap_set;

    virtual void doit(const spot::ltl::atomic_prop* ap);
    virtual void doit(const spot::ltl::constant* c);
    virtual void doit_default(const spot::ltl::formula* f);

};

} /* namespace texada */

#endif /* SUBFORMULAAPCOLLECTOR_H_ */
