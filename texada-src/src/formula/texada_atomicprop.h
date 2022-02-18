/* texada_atomicprop.h
 * Created On: June 25, 2021
 * Author: cjbanks
 */


#ifndef TEXADA_ATOMICPROP_H_
#define TEXADA_ATOMICPROP_H_

#include <string>
#include <stdio.h>
#include <map>
#include <new>
#include <memory>
#include "texadatospotmapping.h"
#include "texada_environment.h"
#include "spot/tl/formula.hh"


namespace texada {
	namespace ltl {

		/**
		 * Creates a mapping from atomic propositions formulated in spot::formula::ap to functions calls
		 * in Texada that rely on atomic propositions. SPOT 2.X implementation of atomic propositions are now
		 * members of the spot::formula object.
		 **/

		class atomic_prop : public formula {
		        public:
		            atomic_prop(){}

		            atomic_prop(const std::string& name, environment& env) :
                                        name_(name), env_(&env), ap_(spot::formula::ap(name)) 
                        {
		                    this->set_spot_formula(spot::formula::ap(name));
                        }

				    atomic_prop(const formula* ap) :
				    name_(ap->spot_formula().ap_name()), ap_(spot::formula::ap(ap->spot_formula()))
				    {
                        this->set_spot_formula(ap->spot_formula());

				    }

				    atomic_prop(spot::formula& ap) :
				    name_(ap.ap_name()), ap_(ap)
				    {
		                this->set_spot_formula(ap);

				    }

				    atomic_prop(const spot::formula& ap) :
				    name_(ap.ap_name()), ap_(ap)
				    {
		                this->set_spot_formula(ap);
				    }


				    static const atomic_prop* instance(const std::string& name, environment& env);

		            const std::string& name() const;

				    environment& env() const;

				    virtual void accept(visitor& v) const;

                    spot::formula ap();

				    std::string name_;

                protected:
                        typedef std::pair<std::string, environment*> pair;
                        typedef std::map<pair, atomic_prop*> map;
                        virtual ~atomic_prop();
                        static map instances;

                private:
                        environment* env_;
                        spot::formula ap_;
                };

	
	}
}
#endif /*TEXADA_ATOMICPROP_H_*/
