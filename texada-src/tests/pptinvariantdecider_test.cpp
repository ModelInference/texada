#include <gtest/gtest.h>
#include "../src/invariant-semantics/pptinvariantdecider.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>


TEST(PptInvariantDeciderTest, EqualToFourImpliesGreaterThanThree){

    texada::ppt_invariant_decider decider = texada::ppt_invariant_decider();
    decider.add_precondition("decl: (declare-const |x| Int) inv: (= |x| 4)");
    decider.add_to_be_proved("decl: (declare-const |x| Int) inv: (>= |x| 3)");
    ASSERT_TRUE(decider.decide());


}
