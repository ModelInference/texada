/*
 * finding.h
 *
 *  Created on: Oct 5, 2014
 *      Author: dennis
 */

#ifndef FINDING_H_
#define FINDING_H_

#include "statistic.h"

namespace texada {

typedef struct finding {
    std::map<std::string, std::string> binding;
    statistic stat;
} finding;

}

#endif /* FINDING_H_ */
