/*
 * arrayinstantitator_test.cpp
 *
 *  Created on: May 21, 2014
 *      Author: clemieux
 */

#include "../src/arrayinstantitator.h"
#include <gtest/gtest.h>
#include <ltlparse/public.hh>
#include <ltlvisit/apcollect.hh>

namespace texada {

TEST(ArrayInstantiatorTest,SimpleTest){
	//Set up to get the atomic proposition map from a formula
	 std::string input = "G(x -> Fy)";
	 spot::ltl::parse_error_list pel;
	 const spot::ltl::formula* f = spot::ltl::parse(input, pel);
	 //std::set<const spot::ltl::atomic_prop*> formula_vars = spot::ltl::atomic_prop_collect(f);

}


} /* namespace texada */
