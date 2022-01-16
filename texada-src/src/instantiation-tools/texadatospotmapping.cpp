/* texadatospotmapping.cpp
 *
 * Created on: December 3, 2021
 * Author: cjbanks
 */


#include "texadatospotmapping.h"



namespace texada {
	namespace ltl {
	    //proposition set for individual formula
		static atomic_prop_set texada_prop_set_map;

		// clone visitor mapping
		clone_visitor::clone_visitor() {}


        const formula* clone_visitor::result() const {
            return result_;
        }


		void clone_visitor::visit(const atomic_prop* ap){
            result_ = ap->clone();
        };

        void clone_visitor::visit(const constant* c){
            result_ = c->clone();
        };

        void clone_visitor::visit(const unop* uo){
            result_ = new unop(uo->op(), recurse(uo->child()));
        };
        void clone_visitor::visit(const binop* bo){
            //std::cout<<"visit binop "<<std::endl;
            result_ = new binop(bo->op(),recurse(bo->first()), recurse(bo->second()));;
        };
        void clone_visitor::visit(const multop* mo){
            //printf("visit multop \n");
            auto mlt_vec = new multop::vec;
            unsigned mos = mo->size();
            for (unsigned i = 0; i< mos; i++)
                mlt_vec->push_back(recurse(mo->nth(i)));
            result_ = new multop(mo->op(), *mlt_vec);
        };
        void clone_visitor::visit(const bunop* bu){
            //printf("visit bunop \n");
            result_ = new bunop(bu->op(), recurse(bu->child()), bu->min(), bu->max());
        };
        void clone_visitor::visit(const formula* f){
            result_ = f->clone();
        };

		const formula* clone_visitor::recurse(const formula* f) {
            f->accept(*this);
			return result_;
		}

		
		
		atomic_prop_set* atomic_prop_collect(const formula* f, atomic_prop_set* texada_prop_set){

			
			if (!texada_prop_set)
			{
				texada_prop_set = new atomic_prop_set;
			}

			std::set<spot::formula> ap_set = *spot::atomic_prop_collect(f->spot_formula());

			for (auto ap : texada_prop_set_map){
			    for (auto &ap_spot : ap_set){
			        if (ap->name() == ap_spot.ap_name()) {
			             texada_prop_set->insert(ap);
			             }
			    }

			}

			return texada_prop_set;
		
		}

		//default environment wrapper	
		const formula* default_environment::require(const std::string& s)
		{
			return atomic_prop::instance(s, *this);
		}
		
		const std::string& default_environment::name() const
		{
			static std::string name("default environment");
			return name;
		}
		
		default_environment::default_environment()
		{

		}

        default_environment::~default_environment() {
            delete this;
        }


		default_environment& default_environment::instance()
		{
			auto singleton = new default_environment(); //std::make_shared<default_environment>(default_environment());
			return *singleton;
		}


		std::string to_string(const formula* node)
		{
            //std::cout<<"LTL TO STRING CALL \n";
			return spot::str_spin_ltl(node->spot_formula());
		}


        formula* constant_formula(const spot::formula& spot_formula)
        {
            switch(spot_formula.kind()){
                case spot::op::tt:
                    return(formula *) new constant(constant::type::True);
                case spot::op::ff:
                    return (formula *) new constant(constant::type::False);
                case spot::op::eword:
                     return (formula*)new constant(constant::type::EmptyWord);
            }
        }

		formula* recurse_form(const spot::formula& f_inner){
		    //local variable children
		    std::vector<spot::formula> children;
		    std::vector<const formula*> children_texada;

		    //check if ap
		    if (f_inner.kind() == spot::op::ap) {

                std::map<std::string, const atomic_prop*> prop_set_names;
                for (auto ap_names: texada_prop_set_map){
                    prop_set_names[ap_names->name()]= ap_names;
                }

                if (prop_set_names.find(f_inner.ap_name()) == prop_set_names.end()) {
                    auto * const ptrf = new atomic_prop(f_inner);
                    texada_prop_set_map.insert(ptrf);
                    return (formula *) ptrf;
                }
                else{
                    auto i_set = prop_set_names.find(f_inner.ap_name());
                 return (formula *) i_set->second;

                }
		    }

		    switch(f_inner.kind()) {
                //check if constant
                case spot::op::tt:
                    return (formula*) constant_formula(f_inner);
                case spot::op::ff:
                    return (formula*) constant_formula(f_inner);
                case spot::op::eword:
                    return (formula*) constant_formula(f_inner);
                    //check if unary
                case spot::op::Not:
                    return (formula*) new unop(unop::type::Not, recurse_form(f_inner.get_child_of(f_inner.kind())));
		        case spot::op::X:
                    return (formula *) new unop(unop::type::X, recurse_form(f_inner.get_child_of(f_inner.kind())));
		        case spot::op::G:
                    return (formula*) new unop(unop::type::G, recurse_form(f_inner.get_child_of(f_inner.kind())));
		        case spot::op::F:
                     return (formula*) new unop(unop::type::F, recurse_form(f_inner.get_child_of(f_inner.kind())));
		        case spot::op::Closure:
		             return(formula*) new unop(unop::type::Closure,
                                                  recurse_form(f_inner.get_child_of(f_inner.kind())));
		        case spot::op::NegClosure:
		             return (formula*) new unop(unop::type::NegClosure,
                                                    recurse_form(f_inner.get_child_of(f_inner.kind())));
		        case spot::op::NegClosureMarked:
		             return (formula*) new unop(unop::type::NegClosureMarked,
                                                         recurse_form(f_inner.get_child_of(f_inner.kind())));
		            //check if binop
		       case spot::op::Xor:
		           for (auto child: f_inner) {
		               children.push_back(child);
		           }
		            return (formula*) new binop(binop::type::Xor, recurse_form(children[0]), recurse_form(children[1]));
		       case spot::op::Implies:
		           for (auto child: f_inner) {
		               children.push_back(child);
		           }
		            return (formula*) new binop(binop::type::Implies, recurse_form(children[0]), recurse_form(children[1]));
		       case spot::op::Equiv:
		           for (auto child: f_inner) {
		               children.push_back(child);
		           }
		            return (formula*) new binop(binop::type::Equiv, recurse_form(children[0]), recurse_form(children[1]));
		       case spot::op::U:
		           for (auto child: f_inner) {
		               children.push_back(child);
		           }
		            return (formula*) new binop(binop::type::U, recurse_form(children[0]), recurse_form(children[1]));
		       case spot::op::R:
		           for (auto child: f_inner) {
		               children.push_back(child);
		           }
		            return (formula*) new binop(binop::type::R, recurse_form(children[0]), recurse_form(children[1]));
		       case spot::op::W:
		           for (auto child: f_inner) {
		               children.push_back(child);
		           }
		            return (formula*) new binop(binop::type::W, recurse_form(children[0]), recurse_form(children[1]));
		       case spot::op::M:
		           for (auto child: f_inner) {
		               children.push_back(child);
		           }
		            return (formula*) new binop(binop::type::M, recurse_form(children[0]), recurse_form(children[1]));
		       case spot::op::EConcat:
		           for (auto child: f_inner) {
		               children.push_back(child);
		           }
		            return (formula*) new binop(binop::type::EConcat, recurse_form(children[0]), recurse_form(children[1]));
		       case spot::op::EConcatMarked:
		           for (auto child: f_inner) {
		               children.push_back(child);
		           }
		            return (formula*) new binop(binop::type::EConcatMarked, recurse_form(children[0]),
                                                  recurse_form(children[1]));
		       case spot::op::UConcat:
		           for (auto child: f_inner) {
		               children.push_back(child);
		           }
		            return (formula*) new binop(binop::type::UConcat, recurse_form(children[0]), recurse_form(children[1]));
		       //check if multop
		       case spot::op::And:
		           for (auto child: f_inner){
		               children_texada.push_back(recurse_form(child));
		           }
		            return (formula*) new multop(multop::type::And, children_texada);
		       case spot::op::Or:
		           for (auto child: f_inner){
                       children_texada.push_back(recurse_form(child));
		           }
		            return (formula*) new multop(multop::type::Or, children_texada);
		       case spot::op::OrRat:
		           for (auto child: f_inner){
                       children_texada.push_back(recurse_form(child));
		           }
		            return (formula*) new multop(multop::type::OrRat, children_texada);
		       case spot::op::AndRat:
		           for (auto child: f_inner){
                       children_texada.push_back(recurse_form(child));
		           }
		            return (formula*) new multop(multop::type::AndRat, children_texada);
		       case spot::op::AndNLM:
		           for (auto child: f_inner){
                       children_texada.push_back(recurse_form(child));
		           }
		            return (formula*) new multop(multop::type::AndNLM, children_texada);
		       case spot::op::Concat:
		           for (auto child: f_inner){
                       children_texada.push_back(recurse_form(child));
		           }
		            return (formula*) new multop(multop::type::Concat, children_texada);
		       case spot::op::Fusion:
		           for (auto child: f_inner){
                       children_texada.push_back(recurse_form(child));
		           }
		            return (formula*) new multop(multop::type::Fusion, children_texada);
		    }
		    }

		formula* parse(const std::string& ltl_string, parse_error_list& error_list)
		{

			try 
			{
				spot::formula formula_temp = spot::parse_formula(ltl_string);

				//error handling for formula of type spot::parsed_formula 
				if (typeid(spot::parsed_formula) == typeid(formula_temp))
				{
                    printf("error thrown\n");
					throw 500;
				}
				return recurse_form(formula_temp);
			}

			catch(int customErr)
			{
			 	spot::parsed_formula formula_parsed_temp = spot::parse_infix_psl(ltl_string);
			        error_list = formula_parsed_temp.errors;

			    static formula parsed_form_type = formula(formula_parsed_temp.f);;
				return &parsed_form_type;
			}
		}
	}
}
