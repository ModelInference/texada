/*
 * jsontreeprinter.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: clemieux
 */

#include "jsontreeprinter.h"

namespace texada {

json_tree_printer::json_tree_printer(std::ostream & of) : outfile(of) {
}

json_tree_printer::~json_tree_printer() {
}

void json_tree_printer::visit(const ltl::atomic_prop *ap){
    outfile << "{\"atomic-prop\" : \"" << ap->name() << "\"}";
}
void json_tree_printer::visit(const ltl::unop *uo){
    outfile << "{";
    switch(uo->op()){
    case ltl::unop::type::F:
        outfile << "\"F\"";

        break;
    case ltl::unop::type::G:
        outfile << "\"G\"";
        break;
    case ltl::unop::type::X:
        outfile << "\"X\"";
        break;
    case ltl::unop::type::Not:
        outfile << "\"!\"";
        break;
    case ltl::unop::type::Closure:
        outfile << "\"Closure\"";
        break;
    case ltl::unop::type::NegClosure:
        outfile << "\"NegClosure\"";
        break;
    case ltl::unop::type::NegClosureMarked:
        outfile << "\"NegClosureMarker\"";
        break;
    case ltl::unop::type::Finish:
        outfile << "\"Finish\"";
        break;
    }
    outfile << " : [";
    uo->child()->accept(*this);
    outfile << "]}";
}

void json_tree_printer::visit(const ltl::binop *bp){
    outfile << "{";
     switch(bp->op()){
     case ltl::binop::type::Equiv:
         outfile << "\"<->\"";
         break;
     case ltl::binop::type::Implies:
         outfile << "\"->\"";
         break;
     case ltl::binop::type::U:
         outfile << "\"U\"";
         break;
     case ltl::binop::type::W:
         outfile << "\"W\"";
         break;
     case ltl::binop::type::R:
         outfile << "\"R\"";
         break;
     case ltl::binop::type::M:
         outfile << "\"M\"";
         break;
     case ltl::binop::type::EConcat:
         outfile << "\"EConcat\"";
         break;
     case ltl::binop::type::EConcatMarked:
        outfile << "\"EConcatMarked\"";
         break;
     case ltl::binop::type::UConcat:
         outfile << "\"UConcat\"";
         break;
     }
     outfile << " : [";
     bp->first()->accept(*this);
     outfile << ", ";
     bp->second()->accept(*this);
     outfile << "]}";
}

void json_tree_printer::visit(const ltl::multop *mo){
    outfile << "{";
    switch(mo->op()){
    case ltl::multop::type::And:
        outfile << "\"And\"";
        break;
    case ltl::multop::type::AndNLM:
        outfile << "\"AndNLM\"";
        break;
    case ltl::multop::type::AndRat:
        outfile << "\"AndRat\"";
        break;
    case ltl::multop::type::Or:
        outfile << "\"Or\"";
        break;
    case ltl::multop::type::OrRat:
        outfile << "\"OrRay\"";
        break;
    case ltl::multop::type::Concat:
        outfile << "\"Concat\"";
        break;
    case ltl::multop::type::Fusion:
        outfile << "\"Fusion\"";
        break;
    }
    outfile << " : [";
    for (int i = 0; i < mo->size(); i++) {
        if (i != 0)
            outfile << ", ";
        mo->nth(i)->accept(*this);
    }
    outfile << "]}";
}
void json_tree_printer::visit(const ltl::constant *c){
    switch(c->val()){
    case ltl::constant::type::False:
        outfile << "false";
        break;
    case ltl::constant::type::True:
        outfile << "true";
        break;
    case ltl::constant::type::EmptyWord:
        outfile << "\"emptyword\"";
        break;
    }

}




} /* namespace texada */
