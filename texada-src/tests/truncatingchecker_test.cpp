/*
 * truncatingchecker_test.cpp
 *
 *  Created on: Jul 15, 2014
 *      Author: clemieux
 */

#include "../src/checkers/truncatingchecker.h"
#include "../src/parsing/simpleparser.h"

#include <gtest/gtest.h>
#include <ltlparse/public.hh>

#include <fstream>

TEST(TruncatingCheckerTest,Simple) {

    //create a set of traces and add them
    std::vector<texada::string_event> trace_vec;
    std::set<std::vector<texada::string_event>> trace_set;

    // create trace a a b b c
    trace_vec.push_back(texada::string_event("a"));
    trace_vec.push_back(texada::string_event("a"));
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event("c"));
    trace_vec.push_back(texada::string_event());
    trace_set.insert(trace_vec);

    // create trace b a b c
    trace_vec.clear();
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event("a"));
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event("c"));
    trace_vec.push_back(texada::string_event());
    trace_set.insert(trace_vec);

    // create trace c b
    trace_vec.clear();
    trace_vec.push_back(texada::string_event("c"));
    trace_vec.push_back(texada::string_event("b"));
    trace_vec.push_back(texada::string_event());
    trace_set.insert(trace_vec);

    // parse the formula
    std::string input = "G(x->XFy)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* afby_form = spot::ltl::parse(input, pel);

    //create event set
    std::set<std::string> unique_events;
    unique_events.insert("a");
    unique_events.insert("b");
    unique_events.insert("c");

    //really the checker is more of a miner because it does instantiation by itself...

    texada::truncating_checker checker = texada::truncating_checker(afby_form,
            unique_events);

    std::vector<std::map<std::string, std::string>> valid_instants =
            checker.return_valid_instants(afby_form, trace_set);

    // should contain a afby b and a afby c
    afby_form->destroy();

    ASSERT_EQ(2, valid_instants.size());
}

TEST(TruncatingCheckerTest,ResourceAllocation) {
    // set up, parse formula and trace
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* prop_type =
            spot::ltl::parse(
                    "(!(y | z) W x) & G((x -> X((!x U z)&(!z U y)))&(y->XFz)&(z->X(!(y | z) W x)))",
                    pel);
    texada::simple_parser * parser = new texada::simple_parser();
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/resource-allocation/abc.txt");
    parser->parse_to_vector(infile);

    //check stuff
    texada::truncating_checker checker = texada::truncating_checker(prop_type,
            parser->return_events());
    std::vector<std::map<std::string, std::string>> valid_instants =
            checker.return_valid_instants(prop_type,
                    parser->return_vec_trace());
    delete parser;
    prop_type->destroy();

    // check only one insant is found
    ASSERT_EQ(valid_instants.size(), 1);

    // now check that the correct instantiation was found
    std::map<std::string, std::string> valid_map = *valid_instants.begin();
    ASSERT_EQ(valid_map.at("x"), "a");
    ASSERT_EQ(valid_map.at("y"), "b");
    ASSERT_EQ(valid_map.at("z"), "c");

}

TEST(TruncatingCheckerTest, STprecedesPafterQ) {

    // set up, parse formula and trace
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* prop_type = spot::ltl::parse(
            "(G!y) | (!y U (y & XFx -> (!x U (a & !x & X(!x U z)))))", pel);
    texada::simple_parser * parser = new texada::simple_parser();
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/resource-allocation/abb4cad.txt");
    parser->parse_to_vector(infile);

    //check stuff
    texada::truncating_checker checker = texada::truncating_checker(prop_type,
            parser->return_events());
    std::vector<std::map<std::string, std::string>> valid_instants =
            checker.return_valid_instants(prop_type,
                    parser->return_vec_trace());

// create the correct instantiation map and the instantiated formula corresponding to it
    std::map<std::string, std::string> inst_map;
    inst_map.insert(std::pair<std::string, std::string>("x", "c"));
    inst_map.insert(std::pair<std::string, std::string>("y", "d"));
    inst_map.insert(std::pair<std::string, std::string>("z", "b"));
    inst_map.insert(std::pair<std::string, std::string>("a", "a"));

// check that the valid instantiations contain the one created above
    bool contains_instantiation = false;
    for (std::vector<std::map<std::string, std::string>>::iterator i =
            valid_instants.begin(); i != valid_instants.end(); i++) {
        std::map<std::string, std::string> current_map = *i;
        if (current_map.at("x") == "c" && current_map.at("y") == "d"
                && current_map.at("z") == "b" && current_map.at("a") == "a") {
            contains_instantiation = true;
            break;
        }
    }
    std::cout << valid_instants.size() << "\n";

    ASSERT_TRUE(contains_instantiation);

    //clean up
    delete parser;
    prop_type->destroy();
}

std::vector<std::map<std::string, std::string>> set_up_trunc_mine(
        std::string form, std::string infilesource) {
    // set up, parse formula and trace
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* prop_type = spot::ltl::parse(form, pel);
    texada::simple_parser * parser = new texada::simple_parser();
    std::ifstream infile(infilesource);
    parser->parse_to_vector(infile);

    //check stuff
    texada::truncating_checker checker = texada::truncating_checker(prop_type,
            parser->return_events());
    return checker.return_valid_instants(prop_type, parser->return_vec_trace());
}

/**
 * Runs the inputted prop type(should be one of the perracotta ones) on a series
 * of traces which match one of the perracotta patterns. Outputs whether the
 * x->a, y->b instantiation of the perracotta pattern/property type represented
 * by formula holds on each of these traces by outputting an array
 * @param formula perracotta property type/pattern in a string
 * @param use_map if true, use the map trace checker instead of the vector checker
 * @return
 */
std::array<bool, 8> set_up_perracotta_tests_trunc(std::string formula) {
    std::array<bool, 8> output_array;
    std::string texada_base = std::string(getenv("TEXADA_HOME"));
    //order of sources
    // Response
    std::string response_source = texada_base + "/traces/testing1/response.txt";
    // Alternating
    std::string alternating_source = texada_base
            + "/traces/testing1/alternating.txt";
    // MultiEffect
    std::string multieffect_source = texada_base
            + "/traces/testing1/multieffect.txt";
    // MultiCause
    std::string multicause_source = texada_base
            + "/traces/testing1/multicause.txt";
    // EffectFirst
    std::string effectfirst_source = texada_base
            + "/traces/testing1/effectfirst.txt";
    // CauseFirst
    std::string causefirst_source = texada_base
            + "/traces/testing1/causefirst.txt";
    // OneCause
    std::string onecause_source = texada_base + "/traces/testing1/onecause.txt";
    // OneEffect
    std::string oneeffect_source = texada_base
            + "/traces/testing1/oneffect.txt";

    //creating the instantiations that should exist
    std::map<std::string, std::string> xtoaytob;
    xtoaytob.insert(std::pair<std::string, std::string>("x", "a"));
    xtoaytob.insert(std::pair<std::string, std::string>("y", "b"));

    // mine the property type in response source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::map<std::string,std::string>> response_set = set_up_trunc_mine(formula, response_source);
    bool containsab = false;
    for (std::vector<std::map<std::string, std::string>>::iterator i =
            response_set.begin(); i != response_set.end(); i++) {
        std::map<std::string, std::string> current_map = *i;
        if (current_map.at("x") == "a" && current_map.at("y") == "b") {
            containsab = true;
            break;
        }
    }

    // add whether it was found to the return val
    output_array[0] = (containsab);

    // mine the property type in alternating source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::map<std::string,std::string>> alternating_set =
            set_up_trunc_mine(formula, alternating_source);
    containsab = false;
    for (std::vector<std::map<std::string, std::string>>::iterator i =
            alternating_set.begin(); i != alternating_set.end(); i++) {
        std::map<std::string, std::string> current_map = *i;
        if (current_map.at("x") == "a" && current_map.at("y") == "b") {
            containsab = true;
            break;
        }
    }
    output_array[1] = (containsab);

    // mine the property type in multieffect source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::map<std::string,std::string>> multieffect_set =
            set_up_trunc_mine(formula, multieffect_source);
    containsab = false;
    for (std::vector<std::map<std::string, std::string>>::iterator i =
            multieffect_set.begin(); i != multieffect_set.end(); i++) {
        std::map<std::string, std::string> current_map = *i;
        if (current_map.at("x") == "a" && current_map.at("y") == "b") {
            containsab = true;
            break;
        }
    }
    // add whether it was found to the return val
    output_array[2] = (containsab);

    // mine the property type in multicause source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::map<std::string,std::string>> multicause_set =
            set_up_trunc_mine(formula, multicause_source);
    containsab = false;
    for (std::vector<std::map<std::string, std::string>>::iterator i =
            multicause_set.begin(); i != multicause_set.end(); i++) {
        std::map<std::string, std::string> current_map = *i;
        if (current_map.at("x") == "a" && current_map.at("y") == "b") {
            containsab = true;
            break;
        }
    }
    // add whether it was found to the return val
    output_array[3] = (containsab);

    // mine the property type in effectfirst source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::map<std::string,std::string>> effectfirst_set =
            set_up_trunc_mine(formula, effectfirst_source);
    containsab = false;
    for (std::vector<std::map<std::string, std::string>>::iterator i =
            effectfirst_set.begin(); i != effectfirst_set.end(); i++) {
        std::map<std::string, std::string> current_map = *i;
        if (current_map.at("x") == "a" && current_map.at("y") == "b") {
            containsab = true;
            break;
        }
    }
    // add whether it was found to the return val
    output_array[4] = (containsab);

    // mine the property type in cause source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::map<std::string,std::string>> causefirst_set =
            set_up_trunc_mine(formula, causefirst_source);
    containsab = false;
    for (std::vector<std::map<std::string, std::string>>::iterator i =
            causefirst_set.begin(); i != causefirst_set.end(); i++) {
        std::map<std::string, std::string> current_map = *i;
        if (current_map.at("x") == "a" && current_map.at("y") == "b") {
            containsab = true;
            break;
        }
    }
    // add whether it was found to the return val
    output_array[5] = (containsab);

    // mine the property type in response source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::map<std::string,std::string>> onecause_set =
            set_up_trunc_mine(formula, onecause_source);
    containsab = false;
    for (std::vector<std::map<std::string, std::string>>::iterator i =
            onecause_set.begin(); i != onecause_set.end(); i++) {
        std::map<std::string, std::string> current_map = *i;
        if (current_map.at("x") == "a" && current_map.at("y") == "b") {
            containsab = true;
            break;
        }
    }
    // add whether it was found to the return val
    output_array[6] = (containsab);

    // mine the property type in oneffect source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::map<std::string,std::string>> oneeffect_set =
            set_up_trunc_mine(formula, oneeffect_source);
    containsab = false;
    for (std::vector<std::map<std::string, std::string>>::iterator i =
            oneeffect_set.begin(); i != oneeffect_set.end(); i++) {
        std::map<std::string, std::string> current_map = *i;
        if (current_map.at("x") == "a" && current_map.at("y") == "b") {
            containsab = true;
            break;
        }
    }
    // add whether it was found to the return val
    output_array[7] = (containsab);

    return output_array;
}

// All of the traces should exhibit the response pattern
TEST(TruncatingCheckerTest, Response) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests_trunc("G(x->XFy)");

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
TEST(TruncatingCheckerTest, Alternating) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests_trunc("((!y)W x)&G((x->X((!x)U y))&(y->X((!y)W x)))");

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
TEST(TruncatingCheckerTest, MultiEffect) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests_trunc("(((!y)W x)&G(x->X((!x)U y)))");

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
TEST(TruncatingCheckerTest, MultiCause) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests_trunc("((!y)W x)&G((x->X(Fy))&(y->X((!y)W x)))");

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
TEST(TruncatingCheckerTest, EffectFirst) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests_trunc("G((x->X((!x)U y))&(y->X((!y)W x)))");

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
TEST(TruncatingCheckerTest, CauseFirst) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests_trunc("((!y)W x)&G(x->XFy)");

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
TEST(TruncatingCheckerTest, OneCause) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests_trunc("G(x->X((!x)U y))");

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
TEST(TruncatingCheckerTest, OneEffect) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests_trunc("G((x->X(Fy))&(y->X((!y)W x)))");

    ASSERT_FALSE(eval_array[0]);
    ASSERT_TRUE(eval_array[1]); //alternating
    ASSERT_FALSE(eval_array[2]);
    ASSERT_TRUE(eval_array[3]); // multicause
    ASSERT_TRUE(eval_array[4]); //effectfirst
    ASSERT_FALSE(eval_array[5]);
    ASSERT_FALSE(eval_array[6]);
    ASSERT_TRUE(eval_array[7]); //oneeffect
}

