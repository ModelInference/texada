/*
 * texadasimplify.h
 * Created On: June 24, 2021
 * Author: cjbanks
 */

#ifndef TEXADASIMPLIFY_H_
#define TEXADASIMPLIFY_H_
#include <spot/tl/simplify.hh>
#include "texadatospotmapping.h"

namespace texada {
	namespace ltl {
		
		//wrapper around tl_simplifier class in SPOT
		class ltl_simplifier {
			public:
				ltl_simplifier(){}
				const formula* negative_normal_form(const formula* f, bool negated=false);
		};

	}
}




#endif /*TEXADASIMPLIFY_H_i*/
