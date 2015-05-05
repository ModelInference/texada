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

    gatherer.set_to_count_map.clear();
    f->accept(gatherer);

    for (std::map<std::set<std::string>, int>::iterator it1 = gatherer.set_to_count_map.begin();
            it1 != gatherer.set_to_count_map.end(); it1++){
        for (std::set<std::string>::iterator it2 = it1->first.begin();
                it2 != it1->first.end(); it2++){
            std::cout << *it2<< " ";
        }
        std::cout << it1->second <<"\n";

    }
    ASSERT_EQ(gatherer.set_to_count_map.size(), 6);

}
