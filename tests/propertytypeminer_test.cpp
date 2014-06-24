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
#include <array>
#include <fstream>
#include "../src/formulainstantiator.h"

std::string response_string = "G(x->XFy)";
std::string alternating_string = "((!y)Wx)&G((x->X((!x)Uy))&(y->X((!y)Wx)))";
std::string multieffect_string = "((!y)Wx)&G(x->X((!x)Uy)))";
std::string multicause_string = "((!y)Wx)&G((x->X(Fy))&(y->X((!y)Wx)))";
std::string effectfirst_string = "G((x->X((!x)Uy))&(y->X((!y)Wx)))";
std::string causefirst_string = "((!y)Wx)&G(x->XFy)";
std::string onecause_string = "G(x->X((!x)Uy)))";
std::string oneeffect_string = "G((x->X(Fy))&(y->X((!y)Wx)))";

std::string texada_base = "/home/clemieux/workspace/texada/Texada";
//TODO: not use definite paths.

TEST(PropertyTypeMinerTest, EventuallyEvent){
	std::set<const spot::ltl::formula*> set =texada::mine_property_type("Fx",
			texada_base + "/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
	ASSERT_EQ(set.size(),10);
}


TEST(PropertyTypeMinerTest, ResourceAllocation){
	std::set<const spot::ltl::formula*> set =texada::mine_property_type("(!(y | z) W x) & G((x -> X((!x U z)&(!z U y)))&(y->XFz)&(z->X(!(y | z) W x)))",
			texada_base + "/traces/resource-allocation/abc.txt");
	ASSERT_EQ(set.size(),1);
}

/*
TEST(PropertyTypeMinerTest, STprecedesPafterQ){
	std::set<const spot::ltl::formula*> set =texada::mine_property_type("(G!y) | (!y U (y & XFx -> (!x U (a & !x & X(!x U z))))",
			texada_base + "/traces/resource-allocation/abb4cad.txt");
	std::map<std::string,std::string> inst_map;
	inst_map.insert(std::pair<std::string,std::string>("x", "c"));
	inst_map.insert(std::pair<std::string,std::string>("y", "d"));
	inst_map.insert(std::pair<std::string,std::string>("z", "b"));
	inst_map.insert(std::pair<std::string,std::string>("a", "a"));
	spot::ltl::parse_error_list pel;

	const spot::ltl::formula * instanted_form= texada::instantiate(spot::ltl::parse("(G!y) | (!y U (y & XFx -> (!x U (a & !x & X(!x U z))))",pel),inst_map);

	bool contains_instated_form = false;
	for (std::set<const spot::ltl::formula*>::iterator i =set.begin() ; i !=set.end(); i++){
		if (*i == instanted_form) {
			contains_instated_form = true;
			break;
		}
	}
	std::cout << set.size() << "\n";

	ASSERT_TRUE(contains_instated_form);
}*/


std::array<bool,8> set_up_perracotta_tests(std::string formula, std::string abstring){
	std::array<bool,8> output_array;
	//order of sources
	// Response
	std::string response_source = texada_base + "/traces/testing1/response.txt";
	// Alternating
	std::string alternating_source = texada_base + "/traces/testing1/alternating.txt";
	// MultiEffect
	std::string multieffect_source = texada_base + "/traces/testing1/multieffect.txt";
	// MultiCause
	std::string multicause_source = texada_base + "/traces/testing1/multicause.txt";
	// EffectFirst
	std::string effectfirst_source = texada_base + "/traces/testing1/effectfirst.txt";
	// CauseFirst
	std::string causefirst_source = texada_base + "/traces/testing1/causefirst.txt";
	// OneCause
	std::string onecause_source = texada_base + "/traces/testing1/onecause.txt";
	// OneEffect
	std::string oneeffect_source = texada_base + "/traces/testing1/oneffect.txt";


	//creating the formulas that should exist

	std::map<std::string,std::string> xtoaytob;
	xtoaytob.insert(std::pair<std::string,std::string>("x", "a"));
	xtoaytob.insert(std::pair<std::string,std::string>("y", "b"));

	spot::ltl::parse_error_list pel;
	const spot::ltl::formula * instanted_form= texada::instantiate(spot::ltl::parse(formula,pel),xtoaytob);
	//std::cout << to_string(instanted_form) << "\n";


	//none of the other
	//std::cout << "mining in response source... \n";
	std::set<const spot::ltl::formula*> response_set =texada::mine_property_type(formula,
				response_source);
	bool containsab = false;
	for (std::set<const spot::ltl::formula*>::iterator it = response_set.begin();
		it !=response_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	output_array[0] = (containsab);


	//std::cout << "mining in alternating source... \n";
	std::set<const spot::ltl::formula*> alternating_set =texada::mine_property_type(formula,
				alternating_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = alternating_set.begin();
		it !=alternating_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	output_array[1] = (containsab);


	//std::cout << "mining in me source... \n";
	std::set<const spot::ltl::formula*> multieffect_set =texada::mine_property_type(formula,
			multieffect_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = multieffect_set.begin();
		it !=multieffect_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	output_array[2] = (containsab);


	//std::cout << "mining in mc source... \n";
	std::set<const spot::ltl::formula*> multicause_set =texada::mine_property_type(formula,
			multicause_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = multicause_set.begin();
		it !=multicause_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	output_array[3] = (containsab);

	//std::cout << "mining in ef source... \n";
	std::set<const spot::ltl::formula*> effectfirst_set =texada::mine_property_type(formula,
			effectfirst_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = effectfirst_set.begin();
		it !=effectfirst_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	output_array[4] = (containsab);

	//std::cout << "mining in cf source... \n";
	std::set<const spot::ltl::formula*> causefirst_set =texada::mine_property_type(formula,
			causefirst_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = causefirst_set.begin();
		it !=causefirst_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	output_array[5] = (containsab);

	//std::cout << "mining in oc source... \n";
	std::set<const spot::ltl::formula*> onecause_set =texada::mine_property_type(formula,
			onecause_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = onecause_set.begin();
		it !=onecause_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	output_array[6] = (containsab);

	//std::cout << "mining in oe source... \n";
	std::set<const spot::ltl::formula*> oneeffect_set =texada::mine_property_type(formula,
			oneeffect_source);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = oneeffect_set.begin();
		it !=oneeffect_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	output_array[7] = (containsab);

	return output_array;
}



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

	std::array<bool,8> eval_array = set_up_perracotta_tests(alternating_string, "(!b W a) & G((b -> X(!b W a)) & (a -> X(!a U b)))");

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
}
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
}
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
}
