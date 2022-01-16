/*
 * texadanenoform.h
 * Created On: June 24, 2021
 * Author: cjbanks
 */


#pragma once

#include <new>
#include <memory>
#include "texadatospotformula.h"
#include "texadatospotmapping.h"
#include <spot/tl/nenoform.hh>

namespace texada {
	namespace ltl {		
		//wrapper around SPOT negative normal form function
		const formula* negative_normal_form(const formula* f, bool negated = false);
	}
}
	

