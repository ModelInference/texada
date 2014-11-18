/*
 * prefixtree_tests.cpp
 *
 *  Created on: Jul 17, 2014
 *      Author: clemieux
 */

#include "../src/trace/prefixtree.h"
#include "../src/checkers/prefixtreechecker.h"
#include "../src/instantiation-tools/subformulaapcollector.h"
#include "../src/parsers/prefixtreeparser.h"
#include <gtest/gtest.h>
#include <ltlparse/public.hh>
#include <fstream>

texada::prefix_tree * create_simple_tree() {
    // create al the sets of trace ids.
    std::set<int> set0123;
    set0123.insert(0);
    set0123.insert(1);
    set0123.insert(2);
    set0123.insert(3);
    std::set<int> set01;
    set01.insert(0);
    set01.insert(1);
    std::set<int> set23;
    set23.insert(2);
    set23.insert(3);
    std::set<int> set0;
    set0.insert(0);
    std::set<int> set1;
    set1.insert(1);
    std::set<int> set2;
    set2.insert(2);
    std::set<int> set3;
    set3.insert(3);

    // this first a is the anchor of it all
    std::shared_ptr<texada::prefix_tree_node> first_a = std::make_shared<
            texada::prefix_tree_node>("a", set0123);
    // first branch
    std::shared_ptr<texada::prefix_tree_node> first_b = std::make_shared<
            texada::prefix_tree_node>("b", set01);
    std::shared_ptr<texada::prefix_tree_node> first_c = std::make_shared<
            texada::prefix_tree_node>("c", set23);

    // first b branch
    std::shared_ptr<texada::prefix_tree_node> c_after_b = std::make_shared<
            texada::prefix_tree_node>("c", set0);
    std::shared_ptr<texada::prefix_tree_node> d_0 = std::make_shared<
            texada::prefix_tree_node>("d", set0);
    std::shared_ptr<texada::prefix_tree_node> terminal_0 = std::make_shared<
            texada::prefix_tree_node>(set0);
    //second b branch
    std::shared_ptr<texada::prefix_tree_node> d_after_b = std::make_shared<
            texada::prefix_tree_node>("d", set1);
    std::shared_ptr<texada::prefix_tree_node> e_1 = std::make_shared<
            texada::prefix_tree_node>("e", set1);
    std::shared_ptr<texada::prefix_tree_node> terminal_1 = std::make_shared<
            texada::prefix_tree_node>(set1);

    //first after c
    std::shared_ptr<texada::prefix_tree_node> e_after_c = std::make_shared<
            texada::prefix_tree_node>("e", set23);
    //first branch at e
    std::shared_ptr<texada::prefix_tree_node> e_2 = std::make_shared<
            texada::prefix_tree_node>("e", set2);
    std::shared_ptr<texada::prefix_tree_node> f_2 = std::make_shared<
            texada::prefix_tree_node>("f", set2);
    std::shared_ptr<texada::prefix_tree_node> terminal_2 = std::make_shared<
            texada::prefix_tree_node>(set2);
    //second branch at e
    std::shared_ptr<texada::prefix_tree_node> d_3 = std::make_shared<
            texada::prefix_tree_node>("d", set3);
    std::shared_ptr<texada::prefix_tree_node> terminal_3 = std::make_shared<
            texada::prefix_tree_node>(set3);

    // now add everything.
    first_a->add_child(first_b);
    first_a->add_child(first_c);

    first_b->add_child(c_after_b);
    c_after_b->add_child(d_0);
    d_0->add_child(terminal_0);

    first_b->add_child(d_after_b);
    d_after_b->add_child(e_1);
    e_1->add_child(terminal_1);

    first_c->add_child(e_after_c);

    e_after_c->add_child(e_2);
    e_2->add_child(f_2);
    f_2->add_child(terminal_2);

    e_after_c->add_child(d_3);
    d_3->add_child(terminal_3);

    // now put it all in a tree
    texada::prefix_tree * all_traces = new texada::prefix_tree();
    all_traces->add_trace(set0123, first_a);

    return all_traces;

}
/**
 * Test prefix tree classes by creating the tree which should be made
 * by these traces:
 * a b c d
 * a b d e
 * a c e e f
 * a c e d
 * That is, those traces should become
 * a-b-c-d
 *  \ \
 *   \ d-e
 *    c-e-e-f
 *       \
 *        d
 */
TEST(PrefixTreeTest,CreateSimpleTree) {

    texada::prefix_tree * all_traces = create_simple_tree();

    /**
     * a-b-c-d
     *  \ \
     *   \ d-e
     *    c-e-e-f
     *       \
     *        d
     */
    // make sure we follow through properly.
    //trace 0
    ASSERT_TRUE(all_traces->get_trace_start(0)->is_satisfied("a"));
    ASSERT_TRUE(all_traces->get_trace_start(0)->get_child(0)->is_satisfied("b"));
    ASSERT_TRUE(all_traces->get_trace_start(0)->get_child(0)->get_child(0)->is_satisfied("c"));
    ASSERT_TRUE(all_traces->get_trace_start(0)->get_child(0)->get_child(0)->get_child(
                    0)->is_satisfied("d"));

    //trace 1
    ASSERT_TRUE(all_traces->get_trace_start(1)->is_satisfied("a"));
    ASSERT_TRUE(all_traces->get_trace_start(1)->get_child(1)->is_satisfied("b"));
    ASSERT_TRUE(all_traces->get_trace_start(1)->get_child(1)->get_child(1)->is_satisfied("d"));
    ASSERT_TRUE(all_traces->get_trace_start(1)->get_child(1)->get_child(1)->get_child(
                    1)->is_satisfied("e"));

    //trace 2
    ASSERT_TRUE(all_traces->get_trace_start(2)->is_satisfied("a"));
    ASSERT_TRUE(all_traces->get_trace_start(2)->get_child(2)->is_satisfied("c"));
    ASSERT_TRUE(all_traces->get_trace_start(2)->get_child(2)->get_child(2)->is_satisfied("e"));
    ASSERT_TRUE(all_traces->get_trace_start(2)->get_child(2)->get_child(2)->get_child(
                    2)->is_satisfied("e"));
    ASSERT_TRUE(all_traces->get_trace_start(2)->get_child(2)->get_child(2)->get_child(
                    2)->get_child(2)->is_satisfied("f"));

    //trace 3
    ASSERT_TRUE(all_traces->get_trace_start(3)->is_satisfied("a"));
    ASSERT_TRUE(all_traces->get_trace_start(3)->get_child(2)->is_satisfied("c"));
    ASSERT_TRUE(all_traces->get_trace_start(3)->get_child(3)->get_child(3)->is_satisfied("e"));
    ASSERT_TRUE(all_traces->get_trace_start(3)->get_child(3)->get_child(3)->get_child(
                    3)->is_satisfied("d"));
    ASSERT_TRUE(all_traces->get_trace_start(3)->get_child(3)->get_child(3)->get_child(
                    3)->get_child(3)->is_satisfied("EndOfTraceVar"));

    delete all_traces;

}

TEST(PrefixTreeCheckerTest, TestSimpleTree) {

    // get simple tree
    texada::prefix_tree * all_traces = create_simple_tree();

    //parse formulae
    spot::ltl::parse_error_list pe_list;
    const spot::ltl::formula * afby_form = spot::ltl::parse("G(a->XFb)",
            pe_list);

    texada::prefix_tree_checker checker;

    ASSERT_FALSE(
            (checker.check_on_trace(afby_form,
                    all_traces->get_trace_start(texada::event("a")))).is_satisfied);

    afby_form->destroy();

    delete all_traces;

}

TEST(PrefixTreeCheckerTest, TestOnTrace) {
    if (getenv("TEXADA_HOME") == NULL) {
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/parsing-tests/simple-pre-tree.txt");

    texada::prefix_tree_parser parser;
    parser.parse(infile);
    std::shared_ptr<texada::prefix_tree> trace_set =
            parser.return_prefix_trees();
    spot::ltl::parse_error_list pe_list;
    const spot::ltl::formula * afby_form = spot::ltl::parse("G(b->XFd)",
            pe_list);
    texada::prefix_tree_checker checker;
    ASSERT_TRUE(
            (checker.check_on_trace(afby_form, trace_set->get_trace_start(texada::event("a")))).is_satisfied);
    afby_form->destroy(); // not sure if this is needed

    afby_form = spot::ltl::parse("G(x->XFy)", pe_list);
    std::map<std::string, std::string> inst_map;
    inst_map.emplace("x", "b");
    inst_map.emplace("y", "d");
    texada::subformula_ap_collector * collector =
            new texada::subformula_ap_collector();
    afby_form->accept(*collector);
    checker.add_relevant_bindings(&collector->subform_ap_set);
    ASSERT_TRUE(
            (checker.check_on_trace(afby_form, trace_set->get_trace_start(texada::event("a")),
                    inst_map)).is_satisfied);
    delete collector;
    afby_form->destroy();

}

TEST(PrefixTreeCheckerTest, RessourceAlloc) {
    if (getenv("TEXADA_HOME") == NULL) {
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::ifstream infile(
            std::string(getenv("TEXADA_HOME"))
                    + "/traces/resource-allocation/pretreeresalloc.txt");

    texada::prefix_tree_parser parser;
    parser.parse(infile);
    std::shared_ptr<texada::prefix_tree> trace_set =
            parser.return_prefix_trees();

    //resource allocation pattern:
    std::string res_alloc =
            "(!(b|c) W a)& G((a->X((!a U c)&(!c U b)))&(c->X(!(b|c) W a)))";

    spot::ltl::parse_error_list pe_list;
    const spot::ltl::formula * resalloc_form = spot::ltl::parse(res_alloc,
            pe_list);

    texada::prefix_tree_checker checker;
    std::shared_ptr<texada::prefix_tree_node> beginning =
            trace_set->get_trace_start(1);

    ASSERT_TRUE((checker.check_on_trace(resalloc_form, beginning)).is_satisfied);
    resalloc_form->destroy();

}
