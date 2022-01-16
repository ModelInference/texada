/*
 * texada_environment.h
 * Created On: June 24, 2021
 * Author: cjbanks
 */

#ifndef TEXADAENVIRONMENT_H_
#define TEXADAENVIRONMENT_H_

#include "texadatospotformula.h"
#include <string>

namespace texada {
	namespace ltl {

                /**
                 * Header for an environment wrapper for Texada function calls. SPOT 2.X no longer requires environments
                 * to assist in object duplicate tracking
                 */
                class environment {

                        public:
                         virtual const formula* require(const std::string& prop_str) = 0;

                         virtual const std::string& name() const = 0;

                         virtual ~environment(){}

                };


	}
}
#endif /*TEXADAENVIRONMENT_H_*/
