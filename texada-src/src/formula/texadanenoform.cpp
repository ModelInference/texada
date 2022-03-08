/*
 * texadanenoform.cpp
 * Created On: June 24, 2021
 * Author: cjbanks
 */

#include "texadanenoform.h"


namespace texada {
	namespace ltl {

		/* maps the SPOT negative normal form function to Texada*/
		const formula* negative_normal_form(const formula* f, bool negated)
		{
            //generate SPOT NNF from formula
			const spot::formula neg_norm_form_spot = spot::negative_normal_form(f->spot_formula(), negated);

            //convert SPOT formula into Texada formula
			const formula* spot_neg_norm_f = recurse_form(neg_norm_form_spot);
			return spot_neg_norm_f;
		}
	}
}
