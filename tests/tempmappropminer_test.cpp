#include "../src/tempmappropminer.h"

#include <gtest/gtest.h>
#include <ltlvisit/tostring.hh>
#include <ltlparse/public.hh>
#include <ltlvisit/nenoform.hh>
#include <array>
#include <fstream>
#include "../src/formulainstantiator.h"

std::string texada_base2 = "/home/clemieux/workspace/texada/Texada";
//TODO: not use definite paths.

TEST(PropertyTypeMinerMapTest, EventuallyEvent){
	std::set<const spot::ltl::formula*> set =texada::mine_map_property_type("Fx",
			texada_base2 + "/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
	ASSERT_EQ(set.size(),10);
}


TEST(PropertyTypeMinerMapTest, ResourceAllocation){
	std::set<const spot::ltl::formula*> set =texada::mine_map_property_type("(!(y | z) W x) & G((x -> X((!x U z)&(!z U y)))&(y->XFz)&(z->X(!(y | z) W x)))",
			texada_base2 + "/traces/resource-allocation/abc.txt");
	spot::ltl::parse_error_list pel;
	std::cout << spot::ltl::to_string(spot::ltl::negative_normal_form
			(spot::ltl::parse("(a -> XFe2) & (e2 -> X((!e2 U a) & (!e2 U e2))) & (e2 -> X(!(a | e2) W e2))",pel),true));
	ASSERT_EQ(set.size(),1);
}
