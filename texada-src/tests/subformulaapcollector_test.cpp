/*
 * subformulaapcollector_test.cpp
 *
 *  Created on: Aug 19, 2014
 *      Author: clemieux
 */

#include <gtest/gtest.h>
#include "../src/instantiation-tools/texadatospotmapping.h"
#include "../src/instantiation-tools/subformulaapcollector.h"

TEST(SubFormulaAPCollectorTest, BasicTraversal) {
    texada::ltl::parse_error_list pe_list;
    const texada::ltl::formula * to_traverse = texada::ltl::parse(
            "G((q & Fr) -> ((!(s & (!r) & X(!r U (t & !r)))) U (r | p)))",
            pe_list);
    texada::subformula_ap_collector * collector = new texada::subformula_ap_collector();
    to_traverse->accept(*collector);
    /**
    for( std::map<const texada::ltl::formula*,std::set<std::string>>::iterator it = collector->subform_ap_set.begin();
            it != collector->subform_ap_set.end(); it++){
        std::cout << texada::ltl::to_string(it->first) << ":\n";
        for (std::set<std::string>::iterator itit = it->second.begin(); itit != it->second.end(); itit++){
            std::cout << *itit << " ";
        }
        std::cout << "\n";
    }
    std::cout << collector->subform_ap_set.size();
    **/
    delete collector;
    to_traverse->destroy();
}

