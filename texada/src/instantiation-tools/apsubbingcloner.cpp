/*
 * apsubbingcloner.cpp
 *
 *  Created on: May 20, 2014
 *      Author: clemieux
 */

#include "apsubbingcloner.h"

#include <ltlvisit/clone.hh>
#include <ltlenv/defaultenv.hh>
#include <map>
#include <iostream>
#include <exception>

namespace texada {

/**
 * Instantiator class to produce instantiated LTL property types.
 * used by texada::instantiate.
 */
class ap_subbing_cloner: public spot::ltl::clone_visitor {

private:
    // Maps each atomic proposition in the formula.
    std::map<std::string, std::string> replacement_map;

public:

    //instantiates the replacement map with the one given
    ap_subbing_cloner(std::map<std::string, std::string>& replacement_map_) :
            replacement_map(replacement_map_) {
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
    const spot::ltl::formula* rename(const spot::ltl::atomic_prop* toreplace) {
        try {
            std::string newname = replacement_map.at(toreplace->name());
            return spot::ltl::default_environment::instance().require(newname);
        } catch (std::exception &e) {
            std::cerr << "Mapping not found for " << toreplace->name() << "."
                    << "Assuming no replacement desired. \n";
            return toreplace;
        }
    }

    using spot::ltl::clone_visitor::visit;

    /**
     * Re-define visiting an atomic proposition as renaming it
     * and cloning instead of simply cloning it.
     * @param ap atomic proposition being visited
     */
    void visit(const spot::ltl::atomic_prop* ap) {
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
const spot::ltl::formula* instantiate(const spot::ltl::formula *node,
        std::map<std::string, std::string>& map) {
    ap_subbing_cloner instantiator = ap_subbing_cloner(map);
    const spot::ltl::formula* return_formula = instantiator.recurse(node);
    return return_formula;
}

} /* namespace texada */
