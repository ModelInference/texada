#include <gtest/gtest.h>
#include "../src/invariant-semantics/pptinvariantdecider.h"
#include "../src/instantiation-tools/constinstantspool.h"
#include "../src/checkers/lineartracechecker.h"
#include "../src/parsers/linearparser.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <stdlib.h>
#include <ltlparse/public.hh>
#include <ltlvisit/apcollect.hh>
#include <ltlvisit/tostring.hh>


TEST(PptInvariantDeciderTest, EqualToFourImpliesGreaterThanThree){

    texada::ppt_invariant_decider decider = texada::ppt_invariant_decider();
    decider.add_precondition("decl: (declare-const |x| Int) inv: (= |x| 4)");
    decider.add_to_be_proved("decl: (declare-const |x| Int) inv: (>= |x| 3)");
    ASSERT_TRUE(decider.decide());
    // test that the other way around doesn't work
    decider.clear();
    decider.add_to_be_proved("decl: (declare-const |x| Int) inv: (= |x| 4)");
    decider.add_precondition("decl: (declare-const |x| Int) inv: (>= |x| 3)");
    ASSERT_FALSE(decider.decide());

}


TEST(PptInvariantDeciderTest, QueueArPpt){
/*
 * Use this ppt:
 * this.currentSize >= 1
    this.back >= 0
    this.front <= size(this.theArray[])-1
    this.back <= size(this.theArray[])-1*/

    texada::ppt_invariant_decider decider = texada::ppt_invariant_decider();
    decider.add_precondition("decl: (declare-const |this.currentSize| Int) inv: (>= |this.currentSize| 1)");
    decider.add_precondition("decl: (declare-const |this.back| Int) inv: (>= |this.currentSize| 0)");
    decider.add_precondition("decl: (declare-const |size(this.theArray[])-1| Int) (declare-const |this.front| Int) inv: (>= |size(this.theArray[])-1| |this.front|)");
    decider.add_precondition("decl: (declare-const |size(this.theArray[])-1| Int) (declare-const |this.back| Int) inv: (>= |size(this.theArray[])-1| |this.back|)");

    decider.add_to_be_proved("decl: (declare-const |this.back| Int) inv: (>= |this.back| 1)");

    ASSERT_FALSE(decider.decide());

}

