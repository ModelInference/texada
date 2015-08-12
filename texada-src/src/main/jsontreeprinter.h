/*
 * jsontreeprinter.h
 *
 *  Created on: Aug 12, 2015
 *      Author: clemieux
 */

#ifndef JSONTREEPRINTER_H_
#define JSONTREEPRINTER_H_

#include <ltlvisit/postfix.hh>
#include <fstream>
#include <ltlast/allnodes.hh>

namespace texada {

// prints an LTL tree in JSON format to the outfile.
class json_tree_printer: public spot::ltl::postfix_visitor {
public:
    json_tree_printer(std::ofstream * of);
    virtual ~json_tree_printer();
    void visit(const spot::ltl::atomic_prop *ap);
    void visit(const spot::ltl::binop *bp);
    void visit(const spot::ltl::unop *uo);
    void visit(const spot::ltl::multop *mo);
    void visit(const spot::ltl::constant *c);
private:
    std::ofstream * outfile;
};

} /* namespace texada */

#endif /* JSONTREEPRINTER_H_ */
