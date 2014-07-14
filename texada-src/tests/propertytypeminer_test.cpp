/*
 * propertytypeminer_test.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: clemieux
 */

#include "../src/main/propertytypeminer.h"
#include <gtest/gtest.h>
#include <ltlvisit/tostring.hh>
#include <ltlparse/public.hh>
#include <array>
#include <fstream>
#include "../src/instantiation-tools/apsubbingcloner.h"

/**
 * Tests the property type miner for correctness, both map and linear tests.
 */

std::string response_string = "G(x->XFy)";
std::string alternating_string = "((!y)W x)&G((x->X((!x)U y))&(y->X((!y)W x)))";
std::string multieffect_string = "(((!y)W x)&G(x->X((!x)U y)))";
std::string multicause_string = "((!y)W x)&G((x->X(Fy))&(y->X((!y)W x)))";
std::string effectfirst_string = "G((x->X((!x)U y))&(y->X((!y)W x)))";
std::string causefirst_string = "((!y)W x)&G(x->XFy)";
std::string onecause_string = "G(x->X((!x)U y))";
std::string oneeffect_string = "G((x->X(Fy))&(y->X((!y)W x)))";

std::string texada_base = std::string(getenv("TEXADA_HOME"));

// Checks that the linear checker finds all finally
// occurrences it should find
TEST(PropertyTypeMinerTest, EventuallyEvent){
	std::set<const spot::ltl::formula*> set =texada::mine_lin_property_type("Fx",
			texada_base + "/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
	ASSERT_EQ(set.size(),10);
}

// Checks that linear checked finds the resource
// allocation pattern in a trace which matches (ab+c)*
TEST(PropertyTypeMinerTest, ResourceAllocation){
	std::set<const spot::ltl::formula*> set =texada::mine_lin_property_type("(!(y | z) W x) & G((x -> X((!x U z)&(!z U y)))&(y->XFz)&(z->X(!(y | z) W x)))",
			texada_base + "/traces/resource-allocation/abc.txt");
	ASSERT_EQ(set.size(),1);
}

// This test works but takes a long time to run:
// checks that miner finds the planted instantiation of
// S and T precede P after Q
/*
TEST(PropertyTypeMinerTest, STprecedesPafterQ){

    // find all valid instantiations of the property type
	std::set<const spot::ltl::formula*> set =texada::mine_lin_property_type("(G!y) | (!y U (y & XFx -> (!x U (a & !x & X(!x U z))))",
			texada_base + "/traces/resource-allocation/abb4cad.txt");

    // create the correct instantiation map and the instantiated formyla corresponding to it
	std::map<std::string,std::string> inst_map;
	inst_map.insert(std::pair<std::string,std::string>("x", "c"));
	inst_map.insert(std::pair<std::string,std::string>("y", "d"));
	inst_map.insert(std::pair<std::string,std::string>("z", "b"));
	inst_map.insert(std::pair<std::string,std::string>("a", "a"));
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula * instanted_form= texada::instantiate(spot::ltl::parse("(G!y) | (!y U (y & XFx -> (!x U (a & !x & X(!x U z))))",pel),inst_map);

    // check that the valid instantiations contain the one created above
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

/**
 * Runs the inputted prop type(should be one of the perracotta ones) on a series
 * of traces which match one of the perracotta patterns. Outputs whether the
 * x->a, y->b instantiation of the perracotta pattern/property type represented
 * by formula holds on each of these traces by outputting an array
 * @param formula perracotta property type/pattern in a string
 * @param use_map if true, use the map trace checker instead of the vector checker
 * @return
 */
std::array<bool,8> set_up_perracotta_tests(std::string formula, bool use_map){
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


	//creating the instantiations that should exist
	std::map<std::string,std::string> xtoaytob;
	xtoaytob.insert(std::pair<std::string,std::string>("x", "a"));
	xtoaytob.insert(std::pair<std::string,std::string>("y", "b"));
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula * instanted_form= texada::instantiate(spot::ltl::parse(formula,pel),xtoaytob);

	// mine the property type in response source, and check
	// that the valid instantiation (instanted_form) is found
	std::set<const spot::ltl::formula*> response_set =texada::mine_property_type(formula,
				response_source,use_map);
	bool containsab = false;
	for (std::set<const spot::ltl::formula*>::iterator it = response_set.begin();
		it !=response_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	// add whether it was found to the return val
	output_array[0] = (containsab);

    // mine the property type in alternating source, and check
    // that the valid instantiation (instanted_form) is found
	std::set<const spot::ltl::formula*> alternating_set =texada::mine_property_type(formula,
				alternating_source,use_map);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = alternating_set.begin();
		it !=alternating_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	output_array[1] = (containsab);

    // mine the property type in multieffect source, and check
    // that the valid instantiation (instanted_form) is found
	std::set<const spot::ltl::formula*> multieffect_set =texada::mine_property_type(formula,
			multieffect_source,use_map);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = multieffect_set.begin();
		it !=multieffect_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	// add whether it was found to the return val
	output_array[2] = (containsab);

    // mine the property type in multicause source, and check
    // that the valid instantiation (instanted_form) is found
	std::set<const spot::ltl::formula*> multicause_set =texada::mine_property_type(formula,
			multicause_source,use_map);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = multicause_set.begin();
		it !=multicause_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	// add whether it was found to the return val
	output_array[3] = (containsab);

    // mine the property type in effectfirst source, and check
    // that the valid instantiation (instanted_form) is found
	std::set<const spot::ltl::formula*> effectfirst_set =texada::mine_property_type(formula,
			effectfirst_source,use_map);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = effectfirst_set.begin();
		it !=effectfirst_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	// add whether it was found to the return val
	output_array[4] = (containsab);

    // mine the property type in cause source, and check
    // that the valid instantiation (instanted_form) is found
	std::set<const spot::ltl::formula*> causefirst_set =texada::mine_property_type(formula,
			causefirst_source,use_map);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = causefirst_set.begin();
		it !=causefirst_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	// add whether it was found to the return val
	output_array[5] = (containsab);

    // mine the property type in response source, and check
    // that the valid instantiation (instanted_form) is found
	std::set<const spot::ltl::formula*> onecause_set =texada::mine_property_type(formula,
			onecause_source,use_map);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = onecause_set.begin();
		it !=onecause_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	// add whether it was found to the return val
	output_array[6] = (containsab);

    // mine the property type in oneffect source, and check
    // that the valid instantiation (instanted_form) is found
	std::set<const spot::ltl::formula*> oneeffect_set =texada::mine_property_type(formula,
			oneeffect_source,use_map);
	containsab =false;
	for (std::set<const spot::ltl::formula*>::iterator it = oneeffect_set.begin();
		it !=oneeffect_set.end(); it++){
		if (*it == instanted_form) {
			containsab = true;
			break;
		}
	}
	// add whether it was found to the return val
	output_array[7] = (containsab);

	return output_array;
}


// All of the traces should exhibit the response pattern
TEST(PropertyTypeMinerTest, Response){

	std::array<bool,8> eval_array = set_up_perracotta_tests(response_string, false);

	ASSERT_TRUE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_TRUE(eval_array[2]);
	ASSERT_TRUE(eval_array[3]);
	ASSERT_TRUE(eval_array[4]);
	ASSERT_TRUE(eval_array[5]);
	ASSERT_TRUE(eval_array[6]);
	ASSERT_TRUE(eval_array[7]);
}

// Only the alternating trace will not violate the alternating property type
TEST(PropertyTypeMinerTest, Alternating){

	std::array<bool,8> eval_array = set_up_perracotta_tests(alternating_string, false);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]); // alternating
	ASSERT_FALSE(eval_array[2]);
	ASSERT_FALSE(eval_array[3]);
	ASSERT_FALSE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}

// multieffect, (ps+)* will also be displayed by the alternating trace,
// since alternating is (ps)*, which matches (ps+)*
TEST(PropertyTypeMinerTest, MultiEffect){

	std::array<bool,8> eval_array = set_up_perracotta_tests(multieffect_string, false);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]); //alternating
	ASSERT_TRUE(eval_array[2]); //multieffect
	ASSERT_FALSE(eval_array[3]);
	ASSERT_FALSE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}


// multicause (p+s)* will be matched by alternating too
TEST(PropertyTypeMinerTest, MultiCause){

	std::array<bool,8> eval_array = set_up_perracotta_tests(multicause_string,false);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]); //alternating
	ASSERT_FALSE(eval_array[2]);
	ASSERT_TRUE(eval_array[3]); //multicause
	ASSERT_FALSE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}

// effectfirst is s*(ps)*, so again, alternating
// matches it
TEST(PropertyTypeMinerTest, EffectFirst){

	std::array<bool,8> eval_array = set_up_perracotta_tests(effectfirst_string,false);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]); // alternating
	ASSERT_FALSE(eval_array[2]);
	ASSERT_FALSE(eval_array[3]);
	ASSERT_TRUE(eval_array[4]); //effectfirst
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}

// causefirst is (p+s+)* so any pattern which does
// not allow effect first matches it
TEST(PropertyTypeMinerTest, CauseFirst){

	std::array<bool,8> eval_array = set_up_perracotta_tests(causefirst_string,false);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]); //alternating
	ASSERT_TRUE(eval_array[2]); //multieffect
	ASSERT_TRUE(eval_array[3]);  //multicause
	ASSERT_FALSE(eval_array[4]);
	ASSERT_TRUE(eval_array[5]); //causefirst
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}

// onecause is s*(ps+)*, so matches alternating,
// multieffect, effectfirst
TEST(PropertyTypeMinerTest, OneCause){

	std::array<bool,8> eval_array = set_up_perracotta_tests(onecause_string,false);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]); //alternating
	ASSERT_TRUE(eval_array[2]); //multieffect
	ASSERT_FALSE(eval_array[3]);
	ASSERT_TRUE(eval_array[4]); //effectfirst
	ASSERT_FALSE(eval_array[5]);
	ASSERT_TRUE(eval_array[6]); // onecause
	ASSERT_FALSE(eval_array[7]);
}

// oneeffect is s*(p+s)*, so matches alternating,
// multicause, effectfirst
TEST(PropertyTypeMinerTest, OneEffect){

	std::array<bool,8> eval_array = set_up_perracotta_tests(oneeffect_string, false);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]); //alternating
	ASSERT_FALSE(eval_array[2]);
	ASSERT_TRUE(eval_array[3]); // multicause
	ASSERT_TRUE(eval_array[4]); //effectfirst
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_TRUE(eval_array[7]); //oneeffect
}

// Checks that the map checker finds all finally
// occurrences it should find
TEST(PropertyTypeMinerMapTest, EventuallyEvent){
	std::set<const spot::ltl::formula*> set =texada::mine_map_property_type("Fx",
			texada_base + "/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
	ASSERT_EQ(set.size(),10);
}

// Checks that map checker finds the resource
// allocation pattern in a trace which matches (ab+c)*
TEST(PropertyTypeMinerMapTest, ResourceAllocation){
	std::set<const spot::ltl::formula*> set =texada::mine_map_property_type("(!(y | z) W x) & G((x -> X((!x U z)&(!z U y)))&(y->XFz)&(z->X(!(y | z) W x)))",
			texada_base + "/traces/resource-allocation/abc.txt");
	spot::ltl::parse_error_list pel;
//	std::cout << spot::ltl::to_string(spot::ltl::negative_normal_form
//			(spot::ltl::parse("(a -> XFe2) & (e2 -> X((!e2 U a) & (!e2 U e2))) & (e2 -> X(!(a | e2) W e2))",pel),true));
	ASSERT_EQ(set.size(),1);
}

// Checks that map checker finds the resource
// allocation pattern in a trace which matches (ab+c)*
TEST(PropertyTypeMinerMapTest, SmallResourceAllocation){
	std::set<const spot::ltl::formula*> set =texada::mine_map_property_type("(!(y | z) W x) & G((x -> X((!x U z)&(!z U y)))&(y->XFz)&(z->X(!(y | z) W x)))",
			texada_base + "/traces/resource-allocation/smallabc.txt");

	ASSERT_EQ(set.size(),1);
}

// This test works but takes a long time to run:
// checks that miner finds the planted instantiation of
// S and T precede P after Q
/*
TEST(PropertyTypeMinerMapTest, STprecedesPafterQ){
    // find all valid instantiations of the property type
	std::set<const spot::ltl::formula*> set =texada::mine_map_property_type("(G!y) | (!y U (y & XFx -> (!x U (a & !x & X(!x U z))))",
			texada_base + "/traces/resource-allocation/abb4cad.txt");

	 // create the correct instantiation map and the instantiated formyla corresponding to it
	std::map<std::string,std::string> inst_map;
	inst_map.insert(std::pair<std::string,std::string>("x", "c"));
	inst_map.insert(std::pair<std::string,std::string>("y", "d"));
	inst_map.insert(std::pair<std::string,std::string>("z", "b"));
	inst_map.insert(std::pair<std::string,std::string>("a", "a"));
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula * instanted_form= texada::instantiate(spot::ltl::parse("(G!y) | (!y U (y & XFx -> (!x U (a & !x & X(!x U z))))",pel),inst_map);

	// check that the valid instantiations contain the one created above
	bool contains_instated_form = false;
	for (std::set<const spot::ltl::formula*>::iterator i =set.begin() ; i !=set.end(); i++){
		if (*i == instanted_form) {
			contains_instated_form = true;
			break;
		}
	}
	std::cout << set.size() << "\n";
	ASSERT_TRUE(contains_instated_form);
}
*/

// All of the traces should exhibit the response pattern
TEST(PropertyTypeMinerMapTest, Response){

	std::array<bool,8> eval_array = set_up_perracotta_tests("G(x->XFy)",true);

	ASSERT_TRUE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_TRUE(eval_array[2]);
	ASSERT_TRUE(eval_array[3]);
	ASSERT_TRUE(eval_array[4]);
	ASSERT_TRUE(eval_array[5]);
	ASSERT_TRUE(eval_array[6]);
	ASSERT_TRUE(eval_array[7]);
}

// Only the alternating trace will not violate the alternating property type
TEST(PropertyTypeMinerMapTest, Alternating){

	std::array<bool,8> eval_array = set_up_perracotta_tests("(!y W x) & G((y -> X(!y W x)) & (x -> X(!x U y)))",true);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]); // alternating
	ASSERT_FALSE(eval_array[2]);
	ASSERT_FALSE(eval_array[3]);
	ASSERT_FALSE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}

// multieffect, (ps+)* will also be displayed by the alternating trace,
// since alternating is (ps)*, which matches (ps+)*
TEST(PropertyTypeMinerMapTest, MultiEffect){

	std::array<bool,8> eval_array = set_up_perracotta_tests("(!y W x) & G(x -> X(!x U y))",true);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_TRUE(eval_array[2]);
	ASSERT_FALSE(eval_array[3]);
	ASSERT_FALSE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}

// multicause (p+s)* will be matched by alternating too
TEST(PropertyTypeMinerMapTest, MultiCause){

	std::array<bool,8> eval_array = set_up_perracotta_tests("(!y W x) & G((x -> XFy) & (y -> X(!y W x)))",true);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_FALSE(eval_array[2]);
	ASSERT_TRUE(eval_array[3]);
	ASSERT_FALSE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}

// effectfirst is s*(ps)*, so again, alternating
// matches it
TEST(PropertyTypeMinerMapTest, EffectFirst){

	std::array<bool,8> eval_array = set_up_perracotta_tests("G((x -> X(!x U y)) & (y -> X(!y W x)))",true);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_FALSE(eval_array[2]);
	ASSERT_FALSE(eval_array[3]);
	ASSERT_TRUE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}

// causefirst is (p+s+)* so any pattern which does
// not allow effect first matches it
TEST(PropertyTypeMinerMapTest, CauseFirst){

	std::array<bool,8> eval_array = set_up_perracotta_tests("G(x -> XFy) & (!y W x)",true);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_TRUE(eval_array[2]);
	ASSERT_TRUE(eval_array[3]);
	ASSERT_FALSE(eval_array[4]);
	ASSERT_TRUE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}

// onecause is s*(ps+)*, so matches alternating,
// multieffect, effectfirst
TEST(PropertyTypeMinerMapTest, OneCause){

	std::array<bool,8> eval_array = set_up_perracotta_tests("G(x -> X(!x U y))",true);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_TRUE(eval_array[2]);
	ASSERT_FALSE(eval_array[3]);
	ASSERT_TRUE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_TRUE(eval_array[6]);
	ASSERT_FALSE(eval_array[7]);
}

// oneeffect is s*(p+s)*, so matches alternating,
// multicause, effectfirst
TEST(PropertyTypeMinerMapTest, OneEffect){

	std::array<bool,8> eval_array = set_up_perracotta_tests("G((x -> XFy) & (y -> X(!y W x)))",true);

	ASSERT_FALSE(eval_array[0]);
	ASSERT_TRUE(eval_array[1]);
	ASSERT_FALSE(eval_array[2]);
	ASSERT_TRUE(eval_array[3]);
	ASSERT_TRUE(eval_array[4]);
	ASSERT_FALSE(eval_array[5]);
	ASSERT_FALSE(eval_array[6]);
	ASSERT_TRUE(eval_array[7]);
}
