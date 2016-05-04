/*
 * constinstantspool_test.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: dennis
 */

#include "../src/instantiation-tools/scopedinstantspoolcreator.h"
#include "../src/checkers/lineartracechecker.h"
#include "../src/checkers/maptracechecker.h"
#include "../src/checkers/prefixtreechecker.h"
#include "../src/checkers/statistic.h"
#include "../src/trace/prefixtree.h"
#include "../src/trace/event.h"
#include <gtest/gtest.h>
#include <ltlparse/public.hh>
#include <ltlvisit/apcollect.hh>

using std::shared_ptr;
using std::map;
using std::string;
using std::set;
using std::vector;

TEST(ScopedInstantsPoolCreatorTest,Basic) {
    //Set up to get the atomic propositions from a property type
    std::string input = "G((\"scope_x\" & x) -> F(\"scope_y\" & y))";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);
    std::shared_ptr<spot::ltl::atomic_prop_set> formula_vars (spot::ltl::atomic_prop_collect(
            f));
    // create a set of events
    std::shared_ptr<std::set<std::string>> events = std::make_shared<
            std::set<std::string>>();
    events->insert("!!!a");
    events->insert("???a??? a < 3");
    events->insert("???a??? a = 4");
    events->insert("!!!b");
    events->insert("???b??? b < 7");
    events->insert("???b??? b = 4");
    events->insert("???b??? b = 47");
    events->insert("!!!a; b");
    events->insert("???a; b??? a + b < 7");
    // now create the event instantiator

    texada::scoped_instants_pool_creator instantiator = texada::scoped_instants_pool_creator(
            events, formula_vars, false, std::vector<std::string>());
    std::shared_ptr<map<string,string>> next_instant = instantiator.get_next_instantiation();
    while (next_instant != NULL){
        for (map<string,string>::iterator it = next_instant->begin() ; it != next_instant->end() ; it++){
            std::cout << it->first << " -> " << it->second << "\n";
        }
        std::cout << "====\n";
        next_instant= instantiator.get_next_instantiation();

    }

    //ASSERT_EQ(, *instantiator.get_next_instantiation());
   // ASSERT_EQ(, *instantiator.get_next_instantiation());
    //ASSERT_EQ(, *instantiator.get_next_instantiation());
   // ASSERT_EQ(returned_array->at(5), *instantiator.get_next_instantiation());
   // ASSERT_EQ(returned_array->at(6), *instantiator.get_next_instantiation());
   // ASSERT_EQ(returned_array->at(7), *instantiator.get_next_instantiation());
   // ASSERT_TRUE(instantiator.get_next_instantiation() == NULL);

    f->destroy();

}
