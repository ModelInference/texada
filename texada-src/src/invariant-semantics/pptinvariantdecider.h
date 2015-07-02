/*
 * pptinvariantdecider.h
 *
 *  Created on: Jul 1, 2015
 *      Author: clemieux
 */

#ifndef PPTINVARIANTDECIDER_H_
#define PPTINVARIANTDECIDER_H_

#include <set>
#include <string>

namespace texada {
using std::set;
using std::string;

/**
 * Decides whether an invariant holds given certain pre-conditions. Expects
 * preconditions and invariant to be proved in the form "decl: (...) inv: (...)"
 */
class ppt_invariant_decider {
public:
    ppt_invariant_decider();
    virtual ~ppt_invariant_decider();
    void add_precondition(string precon);
    void add_to_be_proved(string proved);
    bool decide();
    void clear();
private:
    set<string> declarations;
    set<string> preconditions;
    string to_be_proved;
    void add_all_declarations(string decls);

};

} /* namespace texada */

#endif /* PPTINVARIANTDECIDER_H_ */
