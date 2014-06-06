/*
 * propertytypeminer_test.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: clemieux
 */

#include "../src/propertytypeminer.h"
#include <gtest/gtest.h>
#include <ltlvisit/tostring.hh>
#include <ltlparse/public.hh>
#include <ltlvisit/apcollect.hh>
#include <array>
#include <fstream>
#include "../src/simpleparser.h"
#include "../src/arrayinstantiator.h"
#include "../src/formulatracechecker.h"

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

std::array<bool,8> set_up_perracotta_tests(std::string formula, std::string abstring){
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
	std::string oneeffect_source = "/home/clemieux/workspace/texada/Texada/traces/testing1/oneffect.txt";

	//none of the other
	std::cout << "mining in response source... \n";
	std::set<const spot::ltl::formula*> response_set =texada::mine_property_type(formula,
				response_source);
	bool containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = response_set.begin();
		it !=response_set.end(); it++){
		if (to_string(*it) == abstring) {
			containsab = true;
			break;
		}
	}
	output_array[0] = (containsab);


	std::cout << "mining in alternating source... \n";
	std::set<const spot::ltl::formula*> alternating_set =texada::mine_property_type(formula,
				alternating_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = alternating_set.begin();
		it !=alternating_set.end(); it++){
		if (to_string(*it) == abstring) {
			containsab = true;
			break;
		}
	}
	output_array[1] = (containsab);


	std::cout << "mining in me source... \n";
	std::set<const spot::ltl::formula*> multieffect_set =texada::mine_property_type(formula,
			multieffect_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = multieffect_set.begin();
		it !=multieffect_set.end(); it++){
		if (to_string(*it) == abstring) {
			containsab = true;
			break;
		}
	}
	output_array[2] = (containsab);


	std::cout << "mining in mc source... \n";
	std::set<const spot::ltl::formula*> multicause_set =texada::mine_property_type(formula,
			multicause_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = multicause_set.begin();
		it !=multicause_set.end(); it++){
		if (to_string(*it) == abstring) {
			containsab = true;
			break;
		}
	}
	output_array[3] = (containsab);

	std::cout << "mining in ef source... \n";
	std::set<const spot::ltl::formula*> effectfirst_set =texada::mine_property_type(formula,
			effectfirst_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = effectfirst_set.begin();
		it !=effectfirst_set.end(); it++){
		if (to_string(*it) == abstring) {
			containsab = true;
			break;
		}
	}
	output_array[4] = (containsab);

	std::cout << "mining in cf source... \n";
	std::set<const spot::ltl::formula*> causefirst_set =texada::mine_property_type(formula,
			causefirst_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = causefirst_set.begin();
		it !=causefirst_set.end(); it++){
		if (to_string(*it) == abstring) {
			containsab = true;
			break;
		}
	}
	output_array[5] = (containsab);

	std::cout << "mining in oc source... \n";
	std::set<const spot::ltl::formula*> onecause_set =texada::mine_property_type(formula,
			onecause_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = onecause_set.begin();
		it !=onecause_set.end(); it++){
		if (to_string(*it) == abstring) {
			containsab = true;
			break;
		}
	}
	output_array[6] = (containsab);

	std::cout << "mining in oe source... \n";
	std::set<const spot::ltl::formula*> oneeffect_set =texada::mine_property_type(formula,
			oneeffect_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = oneeffect_set.begin();
		it !=oneeffect_set.end(); it++){
		if (to_string(*it) == abstring) {
			containsab = true;
			break;
		}
	}
	output_array[7] = (containsab);

	return output_array;
}

TEST(BugFindingTest,one){
		//parse the ltl formula
		spot::ltl::parse_error_list pel;
		const spot::ltl::formula* formula = spot::ltl::parse("G((a -> X(!a U b)) & (b -> X(!b W a)))", pel);

		// currently just using simple parser, assumedly could replace this by a
		// more complex parser once we have one
		std::ifstream infile("/home/clemieux/workspace/texada/Texada/traces/testing1/effectfirst.txt");

		texada::simple_parser parser =  texada::simple_parser();
		std::set<std::vector<texada::string_event> >  trace_set = parser.parse(infile);
		std::set<std::string>  event_set = parser.return_events();

		spot::ltl::atomic_prop_set * variables = spot::ltl::atomic_prop_collect(formula);
		//std::cout << "## Number of variables: " <<variables->size() << "\n";
		//create the instantiation array
		texada::array_instantiator instantiator = texada::array_instantiator(event_set, *variables);
		instantiator.instantiate_array();
		std::vector<texada::array_instantiator::inst_fxn> instantiations = instantiator.return_instantiations();

		//debugging stuff below
		/*
		for (std::map<std::string, std::string>::iterator it =instantiations[0].inst_map.begin();
				it !=instantiations[0].inst_map.end(); it++){
			std::cout << it->first << " -> " << it->second << "\n";
		}*/

		//number of events
		int k = event_set.size();
		//number of bindings
		int n = variables->size();
		//size of instnatiations
		int size = instantiations.size();

		//## debugging
		int numvalid;
		int j =0;
		for(std::set<std::vector<texada::string_event> >::iterator it = trace_set.begin();
				it !=trace_set.end(); it++){
			if (j==3){
				std::cout << "iffy pt \n";
			}
			std::vector<texada::string_event> current_vec = *it;
			texada::string_event* current_trace = &current_vec[0];
			texada::check_instants_on_trace(instantiations,formula,current_trace);
			numvalid =0;
			for (int i = 0; i <size; i++){
				if (instantiations[i].validity){
					numvalid++;
				}
			}
			std::cout << "Numvalid: "<< numvalid << "\n";
			j++;
		}

}

/*
TEST(PropertyTypeMinerTest, Response){

	std::array<bool,8> eval_array = set_up_perracotta_tests(response_string, "G(a -> XFb)");

	ASSERT_TRUE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_TRUE(eval_array[2]);
	ASSERT_TRUE(eval_array[3]);
	ASSERT_TRUE(eval_array[4]);
	ASSERT_TRUE(eval_array[5]);
	ASSERT_TRUE(eval_array[6]);
	ASSERT_TRUE(eval_array[7]);
}


TEST(PropertyTypeMinerTest, Alternating){

	std::array<bool,8> eval_array = set_up_perracotta_tests(alternating_string, "(!b W a) & G((a -> X(!a U b)) & (b -> X(!b W a)))");

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_FALSE(eval_array[2]);
	ASSERT_FALSE(eval_array[3]);
	ASSERT_FALSE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}
TEST(PropertyTypeMinerTest, MultiEffect){

	std::array<bool,8> eval_array = set_up_perracotta_tests(multieffect_string, "(!b W a) & G(a -> X(!a U b))");

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_TRUE(eval_array[2]);
	ASSERT_FALSE(eval_array[3]);
	ASSERT_FALSE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}
TEST(PropertyTypeMinerTest, MultiCause){

	std::array<bool,8> eval_array = set_up_perracotta_tests(multicause_string,"(!b W a) & G((a -> XFb) & (b -> X(!b W a)))");

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_FALSE(eval_array[2]);
	ASSERT_TRUE(eval_array[3]);
	ASSERT_FALSE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}*//*
TEST(PropertyTypeMinerTest, EffectFirst){

	std::array<bool,8> eval_array = set_up_perracotta_tests(effectfirst_string,"G((a -> X(!a U b)) & (b -> X(!b W a)))");

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_FALSE(eval_array[2]);
	ASSERT_FALSE(eval_array[3]);
	ASSERT_TRUE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}/*
TEST(PropertyTypeMinerTest, CauseFirst){

	std::array<bool,8> eval_array = set_up_perracotta_tests(causefirst_string,"G(a -> XFb) & (!b W a)");

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_TRUE(eval_array[2]);
	ASSERT_TRUE(eval_array[3]);
	ASSERT_FALSE(eval_array[4]);
	ASSERT_TRUE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}

TEST(PropertyTypeMinerTest, OneCause){

	std::array<bool,8> eval_array = set_up_perracotta_tests(onecause_string,"G(a -> X(!a U b))");

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_TRUE(eval_array[2]);
	ASSERT_FALSE(eval_array[3]);
	ASSERT_TRUE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_TRUE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}

TEST(PropertyTypeMinerTest, OneEffect){

	std::array<bool,8> eval_array = set_up_perracotta_tests(oneeffect_string, "G((a -> XFb) & (b -> X(!b W a)))");

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_FALSE(eval_array[2]);
	ASSERT_TRUE(eval_array[3]);
	ASSERT_TRUE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_TRUE(eval_array[7]);
}*/
