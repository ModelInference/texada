/*
 * jsontreeprinter.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: clemieux
 */

#include "jsontreeprinter.h"

namespace texada {

json_tree_printer::json_tree_printer(std::ofstream * of) {
    outfile = of;
}

json_tree_printer::~json_tree_printer() {
}

void json_tree_printer::visit(const spot::ltl::atomic_prop *ap){
    *outfile << "{\"atomic-prop\" : \"" << ap->name() << "\"}";
}
void json_tree_printer::visit(const spot::ltl::unop *uo){
    *outfile << "{";
    switch(uo->op()){
    case spot::ltl::unop::type::F:
        *outfile << "\"F\"";
        break;
    case spot::ltl::unop::type::G:
        *outfile << "\"G\"";
        break;
    case spot::ltl::unop::type::X:
        *outfile << "\"X\"";
        break;
    case spot::ltl::unop::type::Not:
        *outfile << "\"!\"";
        break;
    case spot::ltl::unop::type::Closure:
        *outfile << "\"Closure\"";
        break;
    case spot::ltl::unop::type::NegClosure:
        *outfile << "\"NegClosure\"";
        break;
    case spot::ltl::unop::type::NegClosureMarked:
        *outfile << "\"NegClosureMarker\"";
        break;
    case spot::ltl::unop::type::Finish:
        *outfile << "\"Finish\"";
        break;
    }
    *outfile << " : [";
    uo->child()->accept(*this);
    *outfile << "]}";
}

void json_tree_printer::visit(const spot::ltl::binop *bp){
    *outfile << "{";
     switch(bp->op()){
     case spot::ltl::binop::type::Equiv:
         *outfile << "\"<->\"";
         break;
     case spot::ltl::binop::type::Implies:
         *outfile << "\"->\"";
         break;
     case spot::ltl::binop::type::U:
         *outfile << "\"U\"";
         break;
     case spot::ltl::binop::type::W:
         *outfile << "\"W\"";
         break;
     case spot::ltl::binop::type::R:
         *outfile << "\"R\"";
         break;
     case spot::ltl::binop::type::M:
         *outfile << "\"M\"";
         break;
     case spot::ltl::binop::type::EConcat:
         *outfile << "\"EConcat\"";
         break;
     case spot::ltl::binop::type::EConcatMarked:
        *outfile << "\"EConcatMarked\"";
         break;
     case spot::ltl::binop::type::UConcat:
         *outfile << "\"UConcat\"";
         break;
     }
     *outfile << " : [";
     bp->first()->accept(*this);
     *outfile << ", ";
     bp->second()->accept(*this);
     *outfile << "]}";
}

void json_tree_printer::visit(const spot::ltl::multop *mo){
    *outfile << "{";
    switch(mo->op()){
    case spot::ltl::multop::type::And:
        *outfile << "\"And\"";
        break;
    case spot::ltl::multop::type::AndNLM:
        *outfile << "\"AndNLM\"";
        break;
    case spot::ltl::multop::type::AndRat:
        *outfile << "\"AndRat\"";
        break;
    case spot::ltl::multop::type::Or:
        *outfile << "\"Or\"";
        break;
    case spot::ltl::multop::type::OrRat:
        *outfile << "\"OrRay\"";
        break;
    case spot::ltl::multop::type::Concat:
        *outfile << "\"Concat\"";
        break;
    case spot::ltl::multop::type::Fusion:
        *outfile << "\"Fusion\"";
        break;
    }
    *outfile << " : [";
    for (int i = 0; i < mo->size(); i++) {
        if (i != 0)
            *outfile << ", ";
        mo->nth(i)->accept(*this);
    }
    *outfile << "]}";
}
void json_tree_printer::visit(const spot::ltl::constant *c){
    switch(c->val()){
    case spot::ltl::constant::type::False:
        *outfile << "false";
        break;
    case spot::ltl::constant::type::True:
        *outfile << "true";
        break;
    case spot::ltl::constant::type::EmptyWord:
        *outfile << "\"emptyword\"";
        break;
    }

}




} /* namespace texada */
