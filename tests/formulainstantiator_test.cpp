/*
 * formulainstantiator_test.cpp
 *
 *  Created on: May 20, 2014
 *      Author: clemieux
 */

#include "../src/formulainstantiator.h"

#include <ltlparse/public.hh>
#include <ltlvisit/tostring.hh>
#include <map>
#include <string>

int main(){

	std::map<std::string, std::string>* map = new std::map<std::string, std::string>();

	map->insert(std::pair<std::string,std::string>("x","a"));
    map->insert(std::pair<std::string,std::string>("y","b"));

    std::string input = "G(x->Fy)";
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);
    std::cout << spot::ltl::to_string(f) << "\n";

    const spot::ltl::formula* instantiatedf = texada::instantiate(f,map);
    std::cout << spot::ltl::to_string(instantiatedf) << "\n";

}
