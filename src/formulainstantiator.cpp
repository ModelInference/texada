/*
 * formulainstantiator.cpp
 *
 *  Created on: May 20, 2014
 *      Author: clemieux
 */

#include "formulainstantiator.h"

#include <ltlvisit/clone.hh>
#include <ltlenv/defaultenv.hh>
#include <map>



namespace texada {


class formula_instantiator : public spot::ltl::clone_visitor{
private:
	std::map<std::string, std::string>* replacement_map;

public:
	formula_instantiator(std::map<std::string,std::string>* replacement_map_):
		replacement_map(replacement_map_) {};
	~formula_instantiator() {};
	const spot::ltl::formula* rename(const spot::ltl::atomic_prop* toreplace){
			std::string newname = replacement_map->at(toreplace->name());
			return spot::ltl::default_environment::instance().require(newname);
		}
	using spot::ltl::clone_visitor::visit;
		void
		visit(const spot::ltl::atomic_prop* ap)
		{
		  result_ = rename(ap);
		}


};

const spot::ltl::formula* instantiate(const spot::ltl::formula *node,
		std::map<std::string, std::string>* map){
	formula_instantiator* instantiator = new formula_instantiator(map);
	return instantiator->recurse(node);
}

} /* namespace texada */
