/*
 * prefixtree_tests.cpp
 *
 *  Created on: Jul 17, 2014
 *      Author: clemieux
 */

#include "../src/parsing/prefixtree.h"
#include "../src/checkers/prefixtreechecker.h"
#include <gtest/gtest.h>
#include <ltlparse/public.hh>


texada::prefix_tree * create_simple_tree(){
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
    texada::prefix_tree_node * first_a = new texada::prefix_tree_node("a", set0123);
    // first branch
    texada::prefix_tree_node * first_b = new texada::prefix_tree_node("b", set01);
    texada::prefix_tree_node * first_c = new texada::prefix_tree_node("c", set23);

    // first b branch
    texada::prefix_tree_node * c_after_b = new texada::prefix_tree_node("c", set0);
    texada::prefix_tree_node * d_0 = new texada::prefix_tree_node("d", set0);
    texada::prefix_tree_node * terminal_0 = new texada::prefix_tree_node(set0);
    //second b branch
    texada::prefix_tree_node * d_after_b = new texada::prefix_tree_node("d", set1);
    texada::prefix_tree_node * e_1 = new texada::prefix_tree_node("e", set1);
    texada::prefix_tree_node * terminal_1 = new texada::prefix_tree_node(set1);

    //first after c
    texada::prefix_tree_node * e_after_c = new texada::prefix_tree_node("e", set23);
    //first branch at e
    texada::prefix_tree_node * e_2 = new texada::prefix_tree_node("e", set2);
    texada::prefix_tree_node * f_2 = new texada::prefix_tree_node("f", set2);
    texada::prefix_tree_node * terminal_2 = new texada::prefix_tree_node(set2);
    //second branch at e
    texada::prefix_tree_node * d_3 = new texada::prefix_tree_node("d", set3);
    texada::prefix_tree_node * terminal_3 = new texada::prefix_tree_node(set3);

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
    std::cout <<  all_traces->get_num_traces() << "\n";

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
    ASSERT_EQ("a", all_traces->get_trace_start(0)->get_name());

    ASSERT_EQ("b", all_traces->get_trace_start(0)->get_child(0)->get_name());
    ASSERT_EQ("c",
            all_traces->get_trace_start(0)->get_child(0)->get_child(0)->get_name());
    ASSERT_EQ("d",
            all_traces->get_trace_start(0)->get_child(0)->get_child(0)->get_child(
                    0)->get_name());

    //trace 1
    ASSERT_EQ("a", all_traces->get_trace_start(1)->get_name());
    ASSERT_EQ("b", all_traces->get_trace_start(1)->get_child(1)->get_name());
    ASSERT_EQ("d",
            all_traces->get_trace_start(1)->get_child(1)->get_child(1)->get_name());
    ASSERT_EQ("e",
            all_traces->get_trace_start(1)->get_child(1)->get_child(1)->get_child(
                    1)->get_name());

    //trace 2
    ASSERT_EQ("a", all_traces->get_trace_start(2)->get_name());
    ASSERT_EQ("c", all_traces->get_trace_start(2)->get_child(2)->get_name());
    ASSERT_EQ("e",
            all_traces->get_trace_start(2)->get_child(2)->get_child(2)->get_name());
    ASSERT_EQ("e",
            all_traces->get_trace_start(2)->get_child(2)->get_child(2)->get_child(
                    2)->get_name());
    ASSERT_EQ("f",
            all_traces->get_trace_start(2)->get_child(2)->get_child(2)->get_child(
                    2)->get_child(2)->get_name());

    //trace 3
    ASSERT_EQ("a", all_traces->get_trace_start(3)->get_name());
    ASSERT_EQ("c", all_traces->get_trace_start(3)->get_child(2)->get_name());
    ASSERT_EQ("e",
            all_traces->get_trace_start(3)->get_child(3)->get_child(3)->get_name());
    ASSERT_EQ("d",
            all_traces->get_trace_start(3)->get_child(3)->get_child(3)->get_child(
                    3)->get_name());
    ASSERT_EQ("EndOfTraceVar",
            all_traces->get_trace_start(3)->get_child(3)->get_child(3)->get_child(
                    3)->get_child(3)->get_name());

    //delete all_traces;

}

TEST(PrefixTreeCheckerTets, TestSimpleTree){

    // get simple tree
    texada::prefix_tree * all_traces = create_simple_tree();

    //parse formulae
    spot::ltl::parse_error_list pe_list;
    const spot::ltl::formula * afby_form = spot::ltl::parse("G(a->XFb)",pe_list);

    texada::prefix_tree_checker checker;

    ASSERT_TRUE(checker.check(afby_form, all_traces->get_trace_start(0), 0));
    ASSERT_TRUE(checker.check(afby_form, all_traces->get_trace_start(1), 1));
    ASSERT_FALSE(checker.check(afby_form, all_traces->get_trace_start(2), 2));
    ASSERT_FALSE(checker.check(afby_form, all_traces->get_trace_start(3), 3));

    afby_form->destroy();

    //delete all_traces;

}

