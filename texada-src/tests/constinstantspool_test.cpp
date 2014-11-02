/*
 * constinstantspool_test.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: dennis
 */

#include "../src/instantiation-tools/constinstantspool.h"
#include "../src/checkers/lineartracechecker.h"
#include "../src/checkers/maptracechecker.h"
#include "../src/checkers/prefixtreechecker.h"
#include "../src/trace/prefixtree.h"
#include "../src/trace/event.h"
#include <gtest/gtest.h>
#include <ltlparse/public.hh>
#include <ltlvisit/apcollect.hh>

/**
 * Checks that const_inst_pool is able to translate an ltl
 * formula into a corresponding map<string,string> mapping
 * the variables in the formula onto themselves.
 */
TEST(ConstInstantsPoolTest,BasicFormula) {
    // Create formula to pass into instantiator
    std::string input = "G(x -> Fy)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);

    texada::const_instants_pool instantiator = texada::const_instants_pool(f);
    std::shared_ptr<std::map<std::string, std::string>> returned_map = instantiator.get_next_instantiation();

    ASSERT_EQ(returned_map->at("x"), "x");
    ASSERT_EQ(returned_map->at("y"), "y");

    f->destroy();
}

/**
 * Tests that checkers are able to use const_inst_pool
 */
TEST(ConstInstantsPoolTest,UsableByLinearChecker) {
    // Create formula to pass into instantiator
    std::string input = "G(a -> FXc)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);

    texada::const_instants_pool* instantiator = new texada::const_instants_pool(f);

    // Create traces to pass into checkers
    std::shared_ptr<std::set<std::vector<texada::event>>> l_traces =
    std::make_shared<std::set<std::vector<texada::event>>>();
    // Traces represent the log: {"a,b,c","b,a,c"}
    texada::event a = texada::event("a");
    texada::event b = texada::event("b");
    texada::event c = texada::event("c");
    texada::event t = texada::event();
    // Linear trace set
    std::vector<texada::event> l_trace1 = {a,b,c,t};
    std::vector<texada::event> l_trace2 = {b,a,c,t};
    l_traces->insert(l_trace1);
    l_traces->insert(l_trace2);

    std::vector<std::map<std::string, std::string>> l_valid_instants = texada::valid_instants_on_traces(f, instantiator,
            l_traces);

    ASSERT_EQ(l_valid_instants.size(), 1);
    ASSERT_EQ(l_valid_instants.at(0).at("a"), "a");
    ASSERT_EQ(l_valid_instants.at(0).at("c"), "c");

    f->destroy();
    delete instantiator;
}

TEST(ConstInstantsPoolTest,UsableByMapChecker) {
    // Create formula to pass into instantiator
    std::string input = "G(a -> FXc)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);

    texada::const_instants_pool* instantiator = new texada::const_instants_pool(f);

    // Create traces to pass into checkers
    std::shared_ptr<std::set<std::map<texada::event, std::vector<long>>>> m_traces =
    std::make_shared<std::set<std::map<texada::event, std::vector<long>>>>();
    // Traces represent the log: {"a,b,c","b,a,c"}
    texada::event a = texada::event("a");
    texada::event b = texada::event("b");
    texada::event c = texada::event("c");
    texada::event t = texada::event();
    // Map trace set
    std::map<texada::event, std::vector<long>> m_trace1;
    std::map<texada::event, std::vector<long>> m_trace2;
    std::vector<long> pos_vec0 {0};
    m_trace1.insert(std::pair<texada::event, std::vector<long>>(a,pos_vec0));
    m_trace2.insert(std::pair<texada::event, std::vector<long>>(b,pos_vec0));
    std::vector<long> pos_vec1 {1};
    m_trace1.insert(std::pair<texada::event, std::vector<long>>(b,pos_vec1));
    m_trace2.insert(std::pair<texada::event, std::vector<long>>(a,pos_vec1));
    std::vector<long> pos_vec2 {2};
    m_trace1.insert(std::pair<texada::event, std::vector<long>>(c,pos_vec2));
    m_trace2.insert(std::pair<texada::event, std::vector<long>>(c,pos_vec2));
    std::vector<long> tpos_vec {3};
    m_trace1.insert(std::pair<texada::event, std::vector<long>>(t,tpos_vec));
    m_trace2.insert(std::pair<texada::event, std::vector<long>>(t,tpos_vec));

    m_traces->insert(m_trace1);
    m_traces->insert(m_trace2);

    std::vector<std::map<std::string, std::string>> m_valid_instants = texada::valid_instants_on_traces(f, instantiator, m_traces);

    ASSERT_EQ(m_valid_instants.size(), 1);
    ASSERT_EQ(m_valid_instants.at(0).at("a"), "a");
    ASSERT_EQ(m_valid_instants.at(0).at("c"), "c");

    f->destroy();
    delete instantiator;
}

TEST(ConstInstantsPoolTest,UsableByPrefixChecker) {
    // Create formula to pass into instantiator
    std::string input = "G(a -> FXc)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);

    texada::const_instants_pool* instantiator = new texada::const_instants_pool(f);

    // Create traces to pass into checkers
    // Traces represent the log: {"a,b,c","a,a,c"}
    std::set<int> set01;
    set01.insert(0);
    set01.insert(1);
    std::set<int> set0;
    set0.insert(0);
    std::set<int> set1;
    set1.insert(1);
    // root
    std::shared_ptr<texada::prefix_tree_node> top = std::make_shared<
            texada::prefix_tree_node>(texada::event("a"), set01);
    // left branch
    std::shared_ptr<texada::prefix_tree_node> middle_left = std::make_shared<
            texada::prefix_tree_node>(texada::event("b"), set0);
    std::shared_ptr<texada::prefix_tree_node> bottom_left = std::make_shared<
            texada::prefix_tree_node>(texada::event("c"), set0);
    std::shared_ptr<texada::prefix_tree_node> terminal_left = std::make_shared<
            texada::prefix_tree_node>(set0);
    // right branch
    std::shared_ptr<texada::prefix_tree_node> middle_right = std::make_shared<
            texada::prefix_tree_node>(texada::event("a"), set1);
    std::shared_ptr<texada::prefix_tree_node> bottom_right = std::make_shared<
            texada::prefix_tree_node>(texada::event("c"), set1);
    std::shared_ptr<texada::prefix_tree_node> terminal_right = std::make_shared<
            texada::prefix_tree_node>(set0);

    // construct tree
    top->add_child(middle_left);
    top->add_child(middle_right);

    middle_left->add_child(bottom_left);
    bottom_left->add_child(terminal_left);

    middle_right->add_child(bottom_right);
    bottom_right->add_child(terminal_right);

    std::shared_ptr<texada::prefix_tree> p_traces = std::make_shared<texada::prefix_tree>();
    p_traces->add_trace(set01, top);

    std::vector<std::map<std::string, std::string>> p_valid_instants = texada::valid_instants_on_traces(f, instantiator,
            p_traces);

    ASSERT_EQ(p_valid_instants.size(), 1);
    ASSERT_EQ(p_valid_instants.at(0).at("a"), "a");
    ASSERT_EQ(p_valid_instants.at(0).at("c"), "c");

    f->destroy();
    delete instantiator;
}

