/*
 * constinstantspool.h
 *
 *  Created on: Aug 28, 2014
 *      Author: dennis
 */

#ifndef CONSTINSTANTSPOOL_H_
#define CONSTINSTANTSPOOL_H_

#include "instantspoolcreator.h"

namespace texada {

/**
 * Instantiation pool creator which generates a single instantiation from a given formula
 */

class const_instants_pool: public instants_pool_creator {
public:
    const_instants_pool(const spot::ltl::formula * prop_type);
    virtual ~const_instants_pool();
    shared_ptr<map<string,string>> get_next_instantiation();

private:
    shared_ptr<map<string,string>> instantiation;

    void instantiate(const spot::ltl::formula * formula);
};

} /* namespace texada */

#endif /* CONSTINSTANTSPOOL_H_ */
