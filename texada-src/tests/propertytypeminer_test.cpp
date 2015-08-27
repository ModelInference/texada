/*
 * propertytypeminer_test.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: clemieux
 */

#include "../src/main/propertytypeminer.h"
#include "../src/main/opts.h"
#include "../src/checkers/statistic.h"
#include <gtest/gtest.h>
#include <ltlvisit/tostring.hh>
#include <ltlparse/public.hh>
#include <array>
#include <fstream>
#include <time.h>
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

// Checks that the linear checker finds all finally
// occurrences it should find
TEST(PropertyTypeMinerTest, EventuallyEvent) {
    if (getenv("TEXADA_HOME") == NULL) {
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }

    std::string texada_base = std::string(getenv("TEXADA_HOME"));
    std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> vec =
            texada::mine_lin_property_type("Fx",
                    texada_base
                            + "/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
    ASSERT_EQ(vec[0].size(), 10);
}

// Checks that linear checked finds the resource
// allocation pattern in a trace which matches (ab+c)*
TEST(PropertyTypeMinerTest, ResourceAllocation) {
    if (getenv("TEXADA_HOME") == NULL) {
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::string texada_base = std::string(getenv("TEXADA_HOME"));

    std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> vec =
            texada::mine_lin_property_type(
                    "(!(y | z) W x) & G((x -> X((!x U z)&(!z U y)))&(y->XFz)&(z->X(!(y | z) W x)))",
                    texada_base + "/traces/resource-allocation/abc.txt");
    ASSERT_EQ(vec[0].size(), 1);

}

template <typename Map>
bool map_compare (Map const &lhs, Map const &rhs) {
    // No predicate needed because there is operator== for pairs already.
    return lhs.size() == rhs.size()
        && std::equal(lhs.begin(), lhs.end(),
                      rhs.begin());
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
std::array<bool, 8> set_up_perracotta_tests(std::string formula, bool use_map) {
    std::array<bool, 8> output_array;

    if (getenv("TEXADA_HOME") == NULL) {
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        return output_array;
    }
    std::string texada_base = std::string(getenv("TEXADA_HOME"));

    std::string trace_type;
    if (use_map) {
        trace_type = "' -m ";
    } else {
        trace_type = "' -l ";
    }

    //order of sources
    // Response
    std::string response_source = texada_base
            + "/property-types/perracotta/response/trace.txt";
    // Alternating
    std::string alternating_source = texada_base
            + "/property-types/perracotta/alternating/trace.txt";
    // MultiEffect
    std::string multieffect_source = texada_base
            + "/property-types/perracotta/multieffect/trace.txt";
    // MultiCause
    std::string multicause_source = texada_base
            + "/property-types/perracotta/multicause/trace.txt";
    // EffectFirst
    std::string effectfirst_source = texada_base
            + "/property-types/perracotta/effectfirst/trace.txt";
    // CauseFirst
    std::string causefirst_source = texada_base
            + "/property-types/perracotta/causefirst/trace.txt";
    // OneCause
    std::string onecause_source = texada_base
            + "/property-types/perracotta/onecause/trace.txt";
    // OneEffect
    std::string oneeffect_source = texada_base
            + "/property-types/perracotta/oneeffect/trace.txt";

    //creating the instantiations that should exist
    std::map<std::string, std::string> xtoaytob;
    xtoaytob.insert(std::pair<std::string, std::string>("x", "a"));
    xtoaytob.insert(std::pair<std::string, std::string>("y", "b"));

    // mine the property type in response source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>> response_set =
            texada::mine_property_type(
                    texada::set_options(
                            "-f '" + formula + trace_type + response_source))[0];
    bool containsab = false;
    for (std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>::iterator it =
            response_set.begin(); it != response_set.end(); it++) {
        if (map_compare((*it).first, xtoaytob)) {
            containsab = true;
        }
    }
    // add whether it was found to the return val
    output_array[0] = (containsab);

    // mine the property type in alternating source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>> alternating_set =
            texada::mine_property_type(
                    texada::set_options(
                            "-f '" + formula + trace_type
                                    + alternating_source))[0];
    containsab = false;
    for (std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>::iterator it =
            alternating_set.begin(); it != alternating_set.end(); it++) {
       if (map_compare((*it).first, xtoaytob))  {
            containsab = true;
        }
    }
    output_array[1] = (containsab);

    // mine the property type in multieffect source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>> multieffect_set =
            texada::mine_property_type(
                    texada::set_options(
                            "-f '" + formula + trace_type
                                    + multieffect_source))[0];
    containsab = false;
    for (std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>::iterator it =
            multieffect_set.begin(); it != multieffect_set.end(); it++) {
       if (map_compare((*it).first, xtoaytob))  {
            containsab = true;
        }
    }
    // add whether it was found to the return val
    output_array[2] = (containsab);

    // mine the property type in multicause source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>> multicause_set =
            texada::mine_property_type(
                    texada::set_options(
                            "-f '" + formula + trace_type + multicause_source))[0];
    containsab = false;
    for (std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>::iterator it =
            multicause_set.begin(); it != multicause_set.end(); it++) {
       if (map_compare((*it).first, xtoaytob))  {
            containsab = true;
        }
    }
    // add whether it was found to the return val
    output_array[3] = (containsab);

    // mine the property type in effectfirst source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>> effectfirst_set =
            texada::mine_property_type(
                    texada::set_options(
                            "-f '" + formula + trace_type
                                    + effectfirst_source))[0];
    containsab = false;
    for (std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>::iterator it =
            effectfirst_set.begin(); it != effectfirst_set.end(); it++) {
       if (map_compare((*it).first, xtoaytob))  {
            containsab = true;
        }
    }
    // add whether it was found to the return val
    output_array[4] = (containsab);

    // mine the property type in cause source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>> causefirst_set =
            texada::mine_property_type(
                    texada::set_options(
                            "-f '" + formula + trace_type + causefirst_source))[0];
    containsab = false;
    for (std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>::iterator it =
            causefirst_set.begin(); it != causefirst_set.end(); it++) {
       if (map_compare((*it).first, xtoaytob))  {
            containsab = true;
        }
    }
    // add whether it was found to the return val
    output_array[5] = (containsab);

    // mine the property type in response source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>> onecause_set =
            texada::mine_property_type(
                    texada::set_options(
                            "-f '" + formula + trace_type + onecause_source))[0];
    containsab = false;
    for (std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>::iterator it =
            onecause_set.begin(); it != onecause_set.end(); it++) {
       if (map_compare((*it).first, xtoaytob))  {
            containsab = true;
        }
    }
    // add whether it was found to the return val
    output_array[6] = (containsab);

    // mine the property type in oneffect source, and check
    // that the valid instantiation (instanted_form) is found
    std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>> oneeffect_set =
            texada::mine_property_type(
                    texada::set_options(
                            "-f '" + formula + trace_type + oneeffect_source))[0];
    containsab = false;
    for (std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>::iterator it =
            oneeffect_set.begin(); it != oneeffect_set.end(); it++) {
       if (map_compare((*it).first, xtoaytob)) {
            containsab = true;
        }
    }
    // add whether it was found to the return val
    output_array[7] = (containsab);


    return output_array;
}

// All of the traces should exhibit the response pattern
TEST(PropertyTypeMinerTest, Response) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(response_string,
            false);

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
TEST(PropertyTypeMinerTest, Alternating) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(alternating_string,
            false);

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
TEST(PropertyTypeMinerTest, MultiEffect) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(multieffect_string,
            false);

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
TEST(PropertyTypeMinerTest, MultiCause) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(multicause_string,
            false);

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
TEST(PropertyTypeMinerTest, EffectFirst) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(effectfirst_string,
            false);

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
TEST(PropertyTypeMinerTest, CauseFirst) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(causefirst_string,
            false);

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
TEST(PropertyTypeMinerTest, OneCause) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(onecause_string,
            false);

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
TEST(PropertyTypeMinerTest, OneEffect) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(oneeffect_string,
            false);

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
TEST(PropertyTypeMinerMapTest, EventuallyEvent) {
    if (getenv("TEXADA_HOME") == NULL) {
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::string texada_base = std::string(getenv("TEXADA_HOME"));

    std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> set =
            texada::mine_map_property_type("Fx",
                    texada_base
                            + "/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
    ASSERT_EQ(set[0].size(), 10);

}

// Checks that map checker finds the resource
// allocation pattern in a trace which matches (ab+c)*
TEST(PropertyTypeMinerMapTest, ResourceAllocation) {
    if (getenv("TEXADA_HOME") == NULL) {
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::string texada_base = std::string(getenv("TEXADA_HOME"));

    std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> set =
            texada::mine_map_property_type(
                    "(!(y | z) W x) & G((x -> X((!x U z)&(!z U y)))&(y->XFz)&(z->X(!(y | z) W x)))",
                    texada_base + "/traces/resource-allocation/abc.txt");
    ASSERT_EQ(set[0].size(), 1);

}

// Checks that map checker finds the resource
// allocation pattern in a trace which matches (ab+c)*
TEST(PropertyTypeMinerMapTest, SmallResourceAllocation) {
    if (getenv("TEXADA_HOME") == NULL) {
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::string texada_base = std::string(getenv("TEXADA_HOME"));

    std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> set =
            texada::mine_map_property_type(
                    "(!(y | z) W x) & G((x -> X((!x U z)&(!z U y)))&(y->XFz)&(z->X(!(y | z) W x)))",
                    texada_base + "/traces/resource-allocation/smallabc.txt");

    ASSERT_EQ(set[0].size(), 1);

}

// All of the traces should exhibit the response pattern
TEST(PropertyTypeMinerMapTest, Response) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests("G(x->XFy)", true);

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
TEST(PropertyTypeMinerMapTest, Alternating) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(
            "(!y W x) & G((y -> X(!y W x)) & (x -> X(!x U y)))", true);

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
TEST(PropertyTypeMinerMapTest, MultiEffect) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(
            "(!y W x) & G(x -> X(!x U y))", true);

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
TEST(PropertyTypeMinerMapTest, MultiCause) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(
            "(!y W x) & G((x -> XFy) & (y -> X(!y W x)))", true);

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
TEST(PropertyTypeMinerMapTest, EffectFirst) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(
            "G((x -> X(!x U y)) & (y -> X(!y W x)))", true);

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
TEST(PropertyTypeMinerMapTest, CauseFirst) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(
            "G(x -> XFy) & (!y W x)", true);

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
TEST(PropertyTypeMinerMapTest, OneCause) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(
            "G(x -> X(!x U y))", true);

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
TEST(PropertyTypeMinerMapTest, OneEffect) {

    std::array<bool, 8> eval_array = set_up_perracotta_tests(
            "G((x -> XFy) & (y -> X(!y W x)))", true);

    ASSERT_FALSE(eval_array[0]);
    ASSERT_TRUE(eval_array[1]);
    ASSERT_FALSE(eval_array[2]);
    ASSERT_TRUE(eval_array[3]);
    ASSERT_TRUE(eval_array[4]);
    ASSERT_FALSE(eval_array[5]);
    ASSERT_FALSE(eval_array[6]);
    ASSERT_TRUE(eval_array[7]);
}

TEST(CheckerEquivalencyTest, DISABLED_STprecedesPafterQ) {
    if (getenv("TEXADA_HOME") == NULL) {
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::string texada_base = std::string(getenv("TEXADA_HOME"));
    // TODO: might need to order these
    // find all valid instantiations of the property type
    std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> set1 =
            texada::mine_property_type(
                    texada::set_options(
                            "-f '(G!y) | (!y U (y & Fx -> (!x U (a & !x & X(!x U z)))))' -l "
                                    + texada_base
                                    + "/traces/resource-allocation/abb4cad.txt"));

    std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> set2 =
            texada::mine_property_type(
                    texada::set_options(
                            "-f '(G!y) | (!y U (y & Fx -> (!x U (a & !x & X(!x U z)))))' -p "
                                    + texada_base
                                    + "/traces/resource-allocation/abb4cad.txt"));

    std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> set3 =
            texada::mine_property_type(
                    texada::set_options(
                            "-f '(G!y) | (!y U (y & Fx -> (!x U (a & !x & X(!x U z)))))' -m "
                                    + texada_base
                                    + "/traces/resource-allocation/abb4cad.txt"));

    ASSERT_EQ(set1[0], set2[0]);
    ASSERT_EQ(set1[0], set3[0]);

    // create the correct instantiation map and the instantiated formula corresponding to it
    std::map<std::string, std::string> inst_map;
    inst_map.insert(std::pair<std::string, std::string>("x", "c"));
    inst_map.insert(std::pair<std::string, std::string>("y", "d"));
    inst_map.insert(std::pair<std::string, std::string>("z", "b"));
    inst_map.insert(std::pair<std::string, std::string>("a", "a"));


    // check that the valid instantiations contain the one created above
    bool contains_instated_form = false;
    for (std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>::iterator i = set1[0].begin();
            i != set1[0].end(); i++) {
        if (map_compare((*i).first, inst_map)) {
            contains_instated_form = true;
            break;
        }
    }

}

// checking that the property type miner works for very large (30,000+ events) traces.
// note: the real purpose of this test is to verify that Texada does not segfault on very large traces
TEST(PropertyTypeMinerTest, LargeTrace) {
    if (getenv("TEXADA_HOME") == NULL) {
            std::cerr << "Error: TEXADA_HOME is undefined. \n";
            FAIL();
        }
        std::string texada_base = std::string(getenv("TEXADA_HOME"));

        // find all valid instantiations along with their full statistics
        std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> set =
                texada::mine_property_type(
                        texada::set_options(
                                "-f 'G(x)' -l "
                                        + texada_base
                                        + "/traces/stress-tests/very-large-trace.txt"));

        // check for correct number of findings
        ASSERT_EQ(1, set[0].size());

}

// checking that the property type miner return correct statistics of findings.
// checks the case when there exist multiple equivalent traces in the log.
TEST(PropertyTypeMinerTest, StatPrint) {
    if (getenv("TEXADA_HOME") == NULL) {
            std::cerr << "Error: TEXADA_HOME is undefined. \n";
            FAIL();
        }
        std::string texada_base = std::string(getenv("TEXADA_HOME"));

        // find all valid instantiations along with their full statistics
        std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> set =
                texada::mine_property_type(
                        texada::set_options(
                                "-f 'G(a -> XFb)' -l -e 'a' -e 'b' --print-stats "
                                        + texada_base
                                        + "/traces/sup-conf-tests/stat-print.txt"));

        // check that the statistics of findings are correct
        ASSERT_EQ(1, set[0].size());
        texada::statistic result = (*set[0].begin()).second;

        //clean up
        ASSERT_EQ(10, result.support);
        ASSERT_EQ(10, result.support_potential);

}

// checking that the property type miner properly filters findings based on confidence threshold
TEST(PropertyTypeMinerTest, ConfidenceFilter) {
    if (getenv("TEXADA_HOME") == NULL) {
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::string texada_base = std::string(getenv("TEXADA_HOME"));

    // find all valid instantiations of the property type
    std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> set =
            texada::mine_property_type(
                    texada::set_options(
                            "-f 'G(x -> XFy)' -l --conf-threshold 0.8 --use-global-thresholds "
                                        + texada_base
                                        + "/traces/sup-conf-tests/conf-filter.txt"));

    // create the correct instantiation map and the instantiated formula corresponding to it
    std::map<std::string, std::string> inst_map1;
    inst_map1.insert(std::pair<std::string, std::string>("x", "a"));
    inst_map1.insert(std::pair<std::string, std::string>("y", "b"));

    std::map<std::string, std::string> inst_map2;
    inst_map2.insert(std::pair<std::string, std::string>("x", "c"));
    inst_map2.insert(std::pair<std::string, std::string>("y", "d"));


    // check that the only valid instantiations are those above
    ASSERT_EQ(2, set[0].size());
    bool contains_instantiation1 = false;
    bool contains_instantiation2 = false;
    for (std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>::iterator i = set[0].begin();
            i != set[0].end(); i++) {
        if (map_compare((*i).first,inst_map1)) {
            contains_instantiation1 = true;
        } else if (map_compare((*i).first,inst_map2)) {
            contains_instantiation2 = true;
        }
    }

    ASSERT_TRUE(contains_instantiation1);
    ASSERT_TRUE(contains_instantiation2);
}

// checking that the property type miner is able to mine multi-propositional logs
TEST(PropertyTypeMinerTest, MultiProp) {
    if (getenv("TEXADA_HOME") == NULL) {
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::string texada_base = std::string(getenv("TEXADA_HOME"));

    // find all valid instantiations of the property type
    std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> set =
            texada::mine_property_type(
                    texada::set_options(
                            "-f 'G(x -> XFy)' -l --parse-mult-prop "
                                        + texada_base
                                        + "/traces/mult-prop-tests/response-pattern-mplog.txt"));

    // create the correct instantiation map and the instantiated formula corresponding to it
    std::map<std::string, std::string> inst_map1;
    inst_map1.insert(std::pair<std::string, std::string>("x", "a"));
    inst_map1.insert(std::pair<std::string, std::string>("y", "b"));

    std::map<std::string, std::string> inst_map2;
    inst_map2.insert(std::pair<std::string, std::string>("x", "c"));
    inst_map2.insert(std::pair<std::string, std::string>("y", "d"));


    // check that the only valid instantiations are those above
    ASSERT_EQ(2, set[0].size());
    bool contains_instantiation1 = false;
    bool contains_instantiation2 = false;
    for (std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>::iterator i = set[0].begin();
            i != set[0].end(); i++) {
        if (map_compare((*i).first, inst_map1)) {
            contains_instantiation1 = true;
        } else if (map_compare((*i).first, inst_map2)) {
            contains_instantiation2 = true;
        }
    }


    ASSERT_TRUE(contains_instantiation1);
    ASSERT_TRUE(contains_instantiation2);
}

bool check_if_contains(std::map<std::string, std::string> map,
        std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>> map_vec){
    for (std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>::iterator i = map_vec.begin();
                   i != map_vec.end(); i++) {
               if (map_compare((*i).first, map)) {
                   return true;
               }
        }
    return false;
}

TEST(PropertyTypeMinerTest, MultiFormulaRegression) {
    if (getenv("TEXADA_HOME") == NULL) {
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::string texada_base = std::string(getenv("TEXADA_HOME"));

    // find all valid instantiations of the property type
    std::vector<std::vector<std::pair<std::map<std::string, std::string>, texada::statistic>>> set =
            texada::mine_property_type(
                    texada::set_options(
                            "-f 'G(x -> XFy)' -f '!y W x' -f Fx -l "
                                        + texada_base
                                        + "/traces/parsing-tests/simple-pre-tree.txt"));
    ASSERT_EQ(set[0].size(), 1);
    ASSERT_EQ(set[1].size(), 7);
    ASSERT_EQ(set[2].size(), 1);

    std::map<std::string, std::string> inst_map;
    inst_map.insert(std::pair<std::string, std::string>("x", "b"));
    inst_map.insert(std::pair<std::string, std::string>("y", "d"));
    ASSERT_TRUE(check_if_contains(inst_map,set[0]));

    inst_map.clear();
    inst_map.insert(std::pair<std::string, std::string>("x", "a"));
    inst_map.insert(std::pair<std::string, std::string>("y", "b"));
    ASSERT_TRUE(check_if_contains(inst_map,set[1]));

    inst_map.clear();
    inst_map.insert(std::pair<std::string, std::string>("x", "a"));
    inst_map.insert(std::pair<std::string, std::string>("y", "c"));
    ASSERT_TRUE(check_if_contains(inst_map,set[1]));

    inst_map.clear();
    inst_map.insert(std::pair<std::string, std::string>("x", "a"));
    inst_map.insert(std::pair<std::string, std::string>("y", "d"));
    ASSERT_TRUE(check_if_contains(inst_map,set[1]));

    inst_map.clear();
    inst_map.insert(std::pair<std::string, std::string>("x", "a"));
    inst_map.insert(std::pair<std::string, std::string>("y", "e"));
    ASSERT_TRUE(check_if_contains(inst_map,set[1]));

    inst_map.clear();
    inst_map.insert(std::pair<std::string, std::string>("x", "a"));
    inst_map.insert(std::pair<std::string, std::string>("y", "f"));
    ASSERT_TRUE(check_if_contains(inst_map,set[1]));

    inst_map.clear();
    inst_map.insert(std::pair<std::string, std::string>("x", "c"));
    inst_map.insert(std::pair<std::string, std::string>("y", "f"));
    ASSERT_TRUE(check_if_contains(inst_map,set[1]));

    inst_map.clear();
    inst_map.insert(std::pair<std::string, std::string>("x", "e"));
    inst_map.insert(std::pair<std::string, std::string>("y", "f"));
    ASSERT_TRUE(check_if_contains(inst_map,set[1]));

    inst_map.clear();
    inst_map.insert(std::pair<std::string, std::string>("x", "a"));
    ASSERT_TRUE(check_if_contains(inst_map,set[2]));

}
