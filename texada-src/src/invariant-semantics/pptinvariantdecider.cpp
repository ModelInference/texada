/*
 * pptinvariantdecider.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: clemieux
 */

#include "pptinvariantdecider.h"
#include <iostream>
#include <map>
#include "z3++.h"

namespace texada {

ppt_invariant_decider::ppt_invariant_decider() {
    // TODO: other set up code to, for example, establish string theories
    to_be_proved = "";
    declarations = set<string>();
    preconditions = set<string>();

}

ppt_invariant_decider::~ppt_invariant_decider() {
    // TODO Auto-generated destructor stub
}

void ppt_invariant_decider::clear() {
    to_be_proved = "";
    declarations.clear();
    preconditions.clear();
}

void ppt_invariant_decider::add_preconditions(set<string> precons){
    for (set<string>::iterator it = precons.begin(); it != precons.end(); it++){
        add_precondition(*it);
    }
}

/**
 * Adds a precondition in the form "decl: (...) inv: (...)" to the object
 * @param precon
 */
void ppt_invariant_decider::add_precondition(string precon) {
    std::size_t inv_loc = precon.find(" inv: ");
    string invariant = precon.substr(inv_loc);
    std::size_t first_paren_loc = invariant.find("(");
    invariant = invariant.substr(first_paren_loc);
    preconditions.insert(invariant);
    string decls = precon.substr(0,inv_loc);
    first_paren_loc = decls.find("(");
    decls = decls.substr(first_paren_loc);
    add_all_declarations(decls);

}

/**
 * Adds all the declarations in the decls string to the set
 * Assumes a string of the form "(decl 1) (decl 2) ... (decl n)"
 * @param decls
 */
void ppt_invariant_decider::add_all_declarations(string decls){
    int paren_count = 0;
    set<string> reserve_set = set<string>();
    int char_pos = 0;
    int paren_begin_pos;
    for (std::string::iterator it = decls.begin(); it != decls.end(); it++){
       if ((*it) == '('){
           if (paren_count == 0){
               paren_begin_pos = char_pos;
           }
           paren_count++;
       }
       else if ((*it) == ')'){
           paren_count--;
           if (paren_count == 0){
               reserve_set.insert(decls.substr(paren_begin_pos, char_pos + 1 - paren_begin_pos));
           }
       }
       char_pos++;
    }
    declarations.insert(reserve_set.begin(), reserve_set.end());
}

/**
 * Adds a statement to be proven in the form "decl: (...) inv: (...)" to the object
 * @param precon
 */
void ppt_invariant_decider::add_to_be_proved(string proved) {
    std::size_t inv_loc = proved.find(" inv: ");
    string invariant = proved.substr(inv_loc);
    std::size_t first_paren_loc = invariant.find("(");
    invariant = invariant.substr(first_paren_loc);
    to_be_proved = invariant;
    string decls = proved.substr(0,inv_loc);
    first_paren_loc = decls.find("(");
    decls = decls.substr(first_paren_loc);
    add_all_declarations(decls);
}

/**
 * Decides whether the preconditions imply the to_be_proved invariant
 * using the z3 API.
 * @return
 */
bool ppt_invariant_decider::decide() {
    string prog = "";
    if (preconditions.size() == 0){
        std::cerr << "No premises to prove from.\n";
        return false;
    }
    if (to_be_proved == ""){
        std::cerr << "Nothing to prove.\n";
        return false;
    }
    for (set<string>::iterator it = declarations.begin(); it != declarations.end() ; it++){
        prog += (*it) + " ";
    }
    string pre;
    if (preconditions.size() > 1){
        pre = "(and";
        for (set<string>::iterator it = preconditions.begin(); it != preconditions.end(); it++){
            //std::cout << *it<< "\n";
            pre += " " + (*it);
        }
        pre += ")";
    }
    else {
        pre = *(preconditions.begin());
      //  std::cout << pre << "\n";
    }
    // prog is !(preconditions->to_be_proved)
    prog += "(assert (and " + pre + " (not " + to_be_proved + ")))";
    //std::cout << prog<< "\n";

    z3::context context;

    Z3_ast parsed_prog = Z3_parse_smtlib2_string(context,prog.c_str(),0,0,0,0,0,0);
    z3::expr e(context, parsed_prog);

    z3::solver solver(context);
    solver.add(e);

    z3::check_result result = solver.check();

    // If !(preconditions->to_be_proved) is satisfiable, then preconditions->to_be_proved
    // is not valid; if it's unsat our desired result is valid.
    if (result == z3::check_result::unsat){
        return true;
    }
    else {
        // here we assume unknown is a false return
        return false;
    }
}

/**
 * Determines whether inv holds at location by using the z3-format logical versions of all
 * events at location (and the inv event) provided in translations. This starts up a z3
 * solver
 * @param location premises, information to be proven from
 * @param inv conclusion we wish to know about (does it hold given premises or not)
 * @param translations provide z3 versions of all events in location and inv
 * @return
 */
bool ap_holds(event location, string inv, shared_ptr<map<string,string>>translations){
     ppt_invariant_decider decider = ppt_invariant_decider();
     set<string> untranslated_props = location.get_props();
     for (set<string>::iterator it = untranslated_props.begin(); it != untranslated_props.end(); it++){
         try{
         decider.add_precondition(translations->at(*it));
         } catch (const std::out_of_range& oor) {
             std::cerr << "Out of Range error: can't find translation for " << *it << '\n';
             throw oor;
         }
     }
     try {
     decider.add_to_be_proved(translations->at(inv));
     } catch (const std::out_of_range& oor) {
         std::cerr << "Out of Range error: can't find translation for " << inv << '\n';
           throw oor;
       }
     return decider.decide();
}

} /* namespace texada */
