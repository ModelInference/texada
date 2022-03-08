/*
 * apsubbingcloner.cpp
 *
 *  Created on: May 20, 2014
 *      Author: clemieux
 */

#include "apsubbingcloner.h"

#include "../formula/texadatospotmapping.h"
#include <map>
#include <iostream>
#include <exception>


namespace texada {

/**
 * Instantiator class to produce instantiated LTL property types.
 * used by texada::instantiate.
 */
class ap_subbing_cloner: public ltl::clone_visitor {

private:
    // Maps each atomic proposition in the formula.
    std::map<std::string, std::string> replacement_map;
    std::vector<std::string> dont_replace;

public:

    //instantiates the replacement map with the one given
    ap_subbing_cloner(std::map<std::string, std::string>& replacement_map_, std::vector<std::string>& not_replaced) :
            replacement_map(replacement_map_), dont_replace(not_replaced) {
    }
    ;
    ~ap_subbing_cloner() {
    }
    ;

    /**
     * Finds the event toreplace is to be replaced by and return the
     * atomic proposition of the same name. If there is no mapping
     * for toreplace, toreplace is not replaced.
     * @param toreplace atomic proposition to be replaced
     * @return atomic proposition toreplace is replaced by, given by
     * replacement map
     */
    const ltl::formula* rename(const ltl::atomic_prop* toreplace) {
        try {
            //std::cout<<  "entered rename for "<< toreplace->name()<< ".\n";
            // if we've arrived at an atomic proposition which represents
            // an event we don't want to replace, we don't replace it.
            for (int i = 0; i < dont_replace.size(); i++){
                if (toreplace->name() == dont_replace[i]){
                    return toreplace->clone();
                }
            }
            std::string newname = replacement_map.at(toreplace->name());
            //std::cout << "Replace " << toreplace->name() << " with " << newname << ".\n";
            const ltl::formula * form = ltl::default_environment::instance().require(newname);
            return form;
        } catch (std::exception &e) {
            std::cerr << "Mapping not found for " << toreplace->name() << ". "
                    << "Assuming no replacement desired. \n";
            return toreplace->clone();

        }
    }

    using ltl::clone_visitor::visit;

    /**
     * Re-define visiting an atomic proposition as renaming it
     * and cloning instead of simply cloning it.
     * @param ap atomic proposition being visited
     */
    void visit(const ltl::atomic_prop* ap) {
        //std::cout << "Visiting atomic prop \n";
        result_ = rename(ap);
    }

};

/**
 * Instantiates node -- that is, turns each atomic proposition/event
 * variables into an atomic proposition with name given in the input map.
 * @param node property type to instantiate
 * @param map mapping from oldvars -> newvars
 * @return node with oldvars replaced with the corresponding newvars
 */
const ltl::formula* instantiate(const ltl::formula *node,
        std::map<std::string, std::string>& map, std::vector<std::string> not_replaced) {
    //std::cout << "Visiting instantiate.\n";
    ap_subbing_cloner instantiator = ap_subbing_cloner(map,not_replaced);
    //std::cout << "Insfjdlka \n";
    return instantiator.recurse(node);
}

} /* namespace texada */
