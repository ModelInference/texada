#include "../src/instantiation-tools/mostoccurringsubsetgatherer.h"
#include "../src/instantiation-tools/orderdecider.h"

#include <ltlparse/public.hh>
#include <ltlvisit/tostring.hh>
#include <map>
#include <string>
#include <set>
#include <gtest/gtest.h>
#include <iostream>


TEST(MostOccurringSubsetGatherer, SmallTests){

    std::string input = "G(x -> Fy)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);

    texada::most_occurring_subset_gatherer gatherer = texada::most_occurring_subset_gatherer();
    f->accept(gatherer);
    ASSERT_EQ(gatherer.set_to_count_map.size(), 1);
    std::set<std::string> set_key;
    set_key.emplace("x");
    set_key.emplace("y");
    ASSERT_EQ(gatherer.set_to_count_map.begin()->first, set_key);
    ASSERT_EQ(gatherer.set_to_count_map.begin()->second, 2);
    f->destroy();

    input = "G ((q & Fr) -> (p -> (!r U (s & !r & !z & X((!r & !z) U t)))) U r)";
    f = spot::ltl::parse(input, pel);
    /*
     * Tree structure:
     *              G
     *              |
     *              ->
     *             / \
     *            &   U
     *          / |   | \
     *         q  F   -> r
     *            |   |\
     *            r   p U
     *                 / \
     *                !   &  _
     *               /  / | \ \
     *              r  s  !  z X
     *                    |     \
     *                    r     U
     *                         / \
     *                        &   t
     *                       / \
     *                      !   !
     *                      r   z
     * so two subformulae have {p,q,r,s,t,z} as a.p.s
     *      1 has {q,r}
     *      2 have {p,r,s,t,z}
     *      2 have {r,s,t,z}
     *      2 have {r,t,z}
     *      1 has  {r,z}
     */

    gatherer.set_to_count_map.clear();
    f->accept(gatherer);
    ASSERT_EQ(gatherer.set_to_count_map.size(), 6);

    set_key.clear();
    set_key.insert("p");
    set_key.insert("q");
    set_key.insert("r");
    set_key.insert("s");
    set_key.insert("t");
    set_key.insert("z");
    ASSERT_FALSE(gatherer.set_to_count_map.find(set_key) == gatherer.set_to_count_map.end());
    ASSERT_EQ(gatherer.set_to_count_map.find(set_key)->second, 2);
    set_key.erase("q");
    ASSERT_FALSE(gatherer.set_to_count_map.find(set_key) == gatherer.set_to_count_map.end());
    ASSERT_EQ(gatherer.set_to_count_map.find(set_key)->second, 2);
    set_key.erase("p");
    ASSERT_FALSE(gatherer.set_to_count_map.find(set_key) == gatherer.set_to_count_map.end());
    ASSERT_EQ(gatherer.set_to_count_map.find(set_key)->second, 2);
    set_key.erase("s");
    ASSERT_FALSE(gatherer.set_to_count_map.find(set_key) == gatherer.set_to_count_map.end());
    ASSERT_EQ(gatherer.set_to_count_map.find(set_key)->second, 2);
    set_key.erase("t");
    ASSERT_FALSE(gatherer.set_to_count_map.find(set_key) == gatherer.set_to_count_map.end());
    ASSERT_EQ(gatherer.set_to_count_map.find(set_key)->second, 1);
    set_key.erase("z");
    set_key.insert("q");
    ASSERT_FALSE(gatherer.set_to_count_map.find(set_key) == gatherer.set_to_count_map.end());
    ASSERT_EQ(gatherer.set_to_count_map.find(set_key)->second, 1);


}


TEST(OrderDecider,BasicTest){
    std::string one[] = {"a","b","c"};
    std::string two[] = {"a","b"};
    std::string three[] = {"b","c"};
    std::set<std::string> first (one, one+3);
    std::set<std::string> second (two, two+2);
    std::set<std::string> third (three, three+2);
    std::map<std::set<std::string>, int> first_map;
    first_map.emplace(first,3);
    first_map.emplace(second,2);
    first_map.emplace(third,1);

    texada::order_decider orderer = texada::order_decider(first_map,3);
    orderer.determine_order();
    ASSERT_EQ(orderer.vars_in_order[0], "b");
    ASSERT_EQ(orderer.vars_in_order[1], "a");
    ASSERT_EQ(orderer.vars_in_order[2], "c");

    first_map.clear();
    first_map.emplace(first,1);
    first_map.emplace(second,2);
    first_map.emplace(third,3);
    texada::order_decider orderer2 = texada::order_decider(first_map,3);
    orderer2.determine_order();
    ASSERT_EQ(orderer2.vars_in_order[0], "b");
    ASSERT_EQ(orderer2.vars_in_order[1], "c");
    ASSERT_EQ(orderer2.vars_in_order[2], "a");

}

TEST(OrderDecider, LargeTest){
    std::string one[] = {"a","b","c","d","e","f"};
    std::string two[] = {"a","b","c"};
    std::string three[] = {"a","b"};
    std::string four[] = {"c","d","e"};
    std::string five[] = {"b","c"};
    std::string six[] = {"d","e"};
    std::string seven[] = {"e","f","a"};
    std::set<std::string> first (one, one + 6);
    std::set<std::string> second (two, two + 3);
    std::set<std::string> third (three, three + 2);
    std::set<std::string> fourth (four, four + 3);
    std::set<std::string> fifth (five, five + 2);
    std::set<std::string> sixth (six, six + 2);
    std::set<std::string> seventh (seven, seven + 3);

    std::map<std::set<std::string>, int> map;
    map.emplace(first, 1);
    map.emplace(second,2);
    map.emplace(third,1);
    map.emplace(fourth,3);
    map.emplace(fifth,2);
    map.emplace(sixth,3);
    map.emplace(seventh,1);

    texada::order_decider orderer = texada::order_decider(map,6);
    orderer.determine_order();

    ASSERT_EQ(orderer.vars_in_order[0], "e");
    ASSERT_EQ(orderer.vars_in_order[1], "d");
    ASSERT_EQ(orderer.vars_in_order[2], "c");
    ASSERT_EQ(orderer.vars_in_order[3], "b");
    ASSERT_EQ(orderer.vars_in_order[4], "a");
    ASSERT_EQ(orderer.vars_in_order[5], "f");

}


TEST(InstantiationOrderTest, IntegrationTest){

       spot::ltl::parse_error_list pel;
       std::string input =  "G ((q & Fr) -> (p -> (!r U (s & !r & !z & X((!r & !z) U t)))) U r)";
       const spot::ltl::formula * f = spot::ltl::parse(input, pel);


       texada::most_occurring_subset_gatherer gatherer = texada::most_occurring_subset_gatherer();
       f->accept(gatherer);

       texada::order_decider orderer = texada::order_decider(gatherer.set_to_count_map,6);
       orderer.determine_order();

       ASSERT_EQ(orderer.vars_in_order[0], "r");
       ASSERT_EQ(orderer.vars_in_order[1], "z");
       ASSERT_EQ(orderer.vars_in_order[2], "t");
       ASSERT_EQ(orderer.vars_in_order[3], "s");
       ASSERT_EQ(orderer.vars_in_order[4], "p");
       ASSERT_EQ(orderer.vars_in_order[5], "q");


}
