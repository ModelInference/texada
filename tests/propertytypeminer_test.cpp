/*
 * propertytypeminer_test.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: clemieux
 */

#include "/home/clemieux/workspace/texada/Texada/src/propertytypeminer.h"
#include <gtest/gtest.h>



TEST(PropertyTypeMinerTest, EventuallyEvent){
	texada::mine_property_type("Fx",
			"/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
}
