/*
 * propertytypeminer_test.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: clemieux
 */

#include "/home/clemieux/workspace/texada/Texada/src/propertytypeminer.h"
#include <gtest/gtest.h>
#include <ltlvisit/tostring.hh>
#include <ltlparse/public.hh>
#include <array>

std::string response_string = "G(x->XFy)";
std::string alternating_string = "((!y)Wx)&G((x->X((!x)Uy))&(y->X((!y)Wx)))";
std::string multieffect_string = "((!y)Wx)&G(x->X((!x)Uy)))";
std::string multicause_string = "((!y)Wx)&G((x->X(Fy))&(y->X((!y)Wx)))";
std::string effectfirst_string = "G((x->X((!x)Uy))&(y->X((!y)Wx)))";
std::string causefirst_string = "((!y)Wx)&G(x->XFy)";
std::string onecause_string = "G(x->X((!x)Uy)))";
std::string oneeffect_string = "G((x->X(Fy))&(y->X((!y)Wx)))";

TEST(PropertyTypeMinerTest, EventuallyEvent){
	std::set<const spot::ltl::formula*> set =texada::mine_property_type("Fx",
			"/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
	ASSERT_EQ(set.size(),10);
}

std::array<bool,8> set_up_perracotta_tests(std::string formula){
	std::array<bool,8> output_array;
	//order of sources
	// Response
	std::string response_source = "/home/clemieux/workspace/texada/Texada/traces/testing1/response.txt";
	// Alternating
	std::string alternating_source = "/home/clemieux/workspace/texada/Texada/traces/testing1/alternating.txt";
	// MultiEffect
	std::string multieffect_source = "/home/clemieux/workspace/texada/Texada/traces/testing1/multieffect.txt";
	// MultiCause
	std::string multicause_source = "/home/clemieux/workspace/texada/Texada/traces/testing1/multicause.txt";
	// EffectFirst
	std::string effectfirst_source = "/home/clemieux/workspace/texada/Texada/traces/testing1/effectfirst.txt";
	// CauseFirst
	std::string causefirst_source = "/home/clemieux/workspace/texada/Texada/traces/testing1/causefirst.txt";
	// OneCause
	std::string onecause_source = "/home/clemieux/workspace/texada/Texada/traces/testing1/onecause.txt";
	// OneEffect
	std::string oneeffect_source = "/home/clemieux/workspace/texada/Texada/traces/testing1/oneeffect.txt";

	//none of the other
	std::set<const spot::ltl::formula*> response_set =texada::mine_property_type(formula,
				response_source);
	output_array[0] = (response_set.size() == 1);

	std::set<const spot::ltl::formula*> alternating_set =texada::mine_property_type(formula,
				alternating_source);

	output_array[1] = (alternating_set.size() == 1);
	std::set<const spot::ltl::formula*> multieffect_set =texada::mine_property_type(formula,
			multieffect_source);

	output_array[2] = (multieffect_set.size() == 1);
	std::set<const spot::ltl::formula*> multicause_set =texada::mine_property_type(formula,
			multicause_source);

	output_array[3] = (multicause_set.size() == 1);
	std::set<const spot::ltl::formula*> response_set =texada::mine_property_type(formula,
				response_source);

	output_array[0] = (response_set.size() == 1);
	std::set<const spot::ltl::formula*> alternating_set =texada::mine_property_type(formula,
				alternating_source);

	output_array[1] = (alternating_set.size() == 1);
	std::set<const spot::ltl::formula*> multieffect_set =texada::mine_property_type(formula,
			multieffect_source);

	output_array[2] = (multieffect_set.size() == 1);
	std::set<const spot::ltl::formula*> multicause_set =texada::mine_property_type(formula,
			multicause_source);

	output_array[1] = (multicause_set.size() == 1);

}

TEST(PropertyTypeMinerTest, Alternating){

	/*std::set<const spot::ltl::formula*> alternating_set =texada::mine_property_type(alternating_string,
				alternating_source);
	std::set<const spot::ltl::formula*> response_set =texada::mine_property_type(alternating_string,
				response_source);
	for (std::set<const spot::ltl::formula*>::iterator it = alternating_set.begin();
			it !=alternating_set.end(); it++){
		std::cout << spot::ltl::to_string(*it) << "\n";

	}
	ASSERT_EQ(alternating_set.size(),1);
	ASSERT_EQ(response_set.size(),0);*/
}
