/*
 * texada_atomicprop.cpp
 * Created On: June 25, 2021
 * Author: cjbanks
 */

#include "texada_atomicprop.h"

namespace texada {
	namespace ltl {


        //track instances of atomic propositions
		atomic_prop::map atomic_prop::instances;


		//returns an instance of the atomic proposition with input: name (string), env (environment) 
		const atomic_prop* atomic_prop::instance(const std::string& name, environment& env)
                {
                    pair p(name, &env);
                    const atomic_prop* ap;
                    std::pair<map::iterator, bool> ires = instances.insert(map::value_type(p, 0));
                    if (!ires.second)
                        ap = ires.first->second;
                    else
                        ap = ires.first->second = new atomic_prop(name, env);
                    ap->clone();
                    return ap;
                }


         //returns the name of the atomic proposition
		 const std::string& atomic_prop::name() const
		{
		    if (name_ == ""){
		        printf("atomic prop -- name empty \n");
		        spot::formula form = this->spot_formula();
		        static std::string name_ap = form.ap_name();
		        //std::cout << "atomic prop --name: " << name_ap << "\n";
		        return name_ap;
		    }
		    else {
		        //std::cout<< "name in AP: "<<  name_ <<"\n";
		        return name_;
		    }
		}


		environment& atomic_prop::env() const
		{
			return *env_;
		}


        //clones the atomic proposition pointer in clone_visitor class
		void atomic_prop::accept(visitor& v) const
		{
			v.visit(this);
		}

        //returns the SPOT atomic proposition object
        spot::formula atomic_prop::ap(){

            return ap_;
        }

		atomic_prop::~atomic_prop()
		{
            // Get this instance out of the instance map.
            pair p(name(), &env());
            auto i = instances.find(p);
            assert (i != instances.end());
            instances.erase(i);
        }

	}
}
