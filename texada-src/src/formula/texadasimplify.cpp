/*
 * texadasimplify.cpp
 * Created On: June 24, 2021
 * Author: cjbanks
 */

#include "texadasimplify.h"

namespace texada {
	namespace ltl {
		const formula* ltl_simplifier::negative_normal_form(const formula* f, bool negated)
		{
			spot::tl_simplifier simplifier;
			const spot::formula simplified_form = simplifier.negative_normal_form(f->spot_formula(), negated);
			return reinterpret_cast<const formula *>(recurse_form(simplified_form));
		}

	}
}
