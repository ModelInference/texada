/*
 * jsontreeprinter.h
 *
 *  Created on: Aug 12, 2015
 *      Author: clemieux
 */

#ifndef JSONTREEPRINTER_H_
#define JSONTREEPRINTER_H_

#include <fstream>
#include "../instantiation-tools/texadapostfix.h"
#include "../instantiation-tools/texada_allnodes.h"

namespace texada {

// prints an LTL tree in JSON format to the outfile.
class json_tree_printer: public ltl::postfix_visitor {
public:
    json_tree_printer(std::ostream & of);
    virtual ~json_tree_printer();
    void visit(const ltl::atomic_prop *ap);
    void visit(const ltl::binop *bp);
    void visit(const ltl::unop *uo);
    void visit(const ltl::multop *mo);
    void visit(const ltl::constant *c);
private:
    std::ostream & outfile;
};

} /* namespace texada */

#endif /* JSONTREEPRINTER_H_ */
