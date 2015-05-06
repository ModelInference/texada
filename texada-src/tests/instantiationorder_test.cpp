#include "../src/instantiation-tools/mostoccurringsubsetgatherer.h"

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
