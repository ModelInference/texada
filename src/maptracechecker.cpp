/*
 * maptracechecker.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: clemieux
 */

#include "maptracechecker.h"
#include <ltlvisit/nenoform.hh>

namespace texada {

map_trace_checker::map_trace_checker(std::map<string_event,std::vector<long>> trace_map_):
	trace_map(trace_map_){

}

map_trace_checker::~map_trace_checker() {
	// TODO Auto-generated destructor stub
}

bool map_trace_checker::check_on_trace(const spot::ltl::formula* node){
	interval base_interval;
	return check(node, base_interval);

}

bool map_trace_checker::check(const spot::ltl::formula* node, interval intvl){
	switch (node->kind()){
	case spot::ltl::formula::Constant:
		return check(static_cast<const spot::ltl::constant*>(node), intvl);
	case spot::ltl::formula::AtomicProp:
		return check(static_cast<const spot::ltl::atomic_prop*>(node),intvl);
	case spot::ltl::formula::UnOp:
		return check(static_cast<const spot::ltl::unop*>(node),intvl);
	case spot::ltl::formula::BinOp:
		return check(static_cast<const spot::ltl::binop*>(node)intvl);
	case spot::ltl::formula::MultOp:
		return check(static_cast<const spot::ltl::multop*>(node),intvl);
	case spot::ltl::formula::BUnOp:
		return check(static_cast<const spot::ltl::bunop*>(node));
	case spot::ltl::formula::AutomatOp:
		return check(static_cast<const spot::ltl::automatop*>(node));
	default:
		return false;
	}

}

bool map_trace_checker::check(const spot::ltl::constant* node, interval intvl){
	spot::ltl::constant::type value = node->val();
	switch (value){
	case spot::ltl::constant::True:
		return true;
	case spot::ltl::constant::False:
		return false;
	case spot::ltl::constant::EmptyWord:
		std::cerr << "We came across the empty word. Returning vacuously true. \n" ;
		return true;
	default:
		return false;
	}
}

bool map_trace_checker::check(const spot::ltl::atomic_prop* node, interval intvl){
	long first_occurrence = find_first_occurrence(node, intvl);
	if (intvl.start == first_occurrence) return true;
	else return false;

}

bool map_trace_checker::check(const spot::ltl::unop* node, interval intvl){
	spot::ltl::unop::type optype = node->op();

		switch (optype){

		// Globally case
		case spot::ltl::unop::G:{
			long first_occ = find_first_occurence(spot::ltl::negative_normal_form(node,true), intvl);
		}
		// Finally case
		case spot::ltl::unop::F:

		// Next case
		case spot::ltl::unop::X:

		// Not case
		case spot::ltl::unop::Not:


		// Other operators are not LTL, don't support them
		default:
			std::cerr << "Unsupported unary operator. Returning false. \n" ;
			return false;

		}
}

long map_trace_checker::find_first_occurrence(const spot::ltl::formula* node,interval intvl){
	return -1;
}
/**
 * Finds the first occurrence of an atomic proposition in a given interval
 * @param node atomic prop to find
 * @param intvl interval to search in
 * @return first occurence position, -1 if not found.
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::atomic_prop* node,interval intvl){
	// REQUIRES: to_search is sorted. this should be assured earlier on.
	std::vector<long> to_search = trace_map.at(string_event(node->name(),false));
	long left = 0;
	long right = to_search.size();
	long newpos;
	while (true){
		newpos = (right + left ) / 2;
		if (to_search[newpos] < intvl.start){
			/**
			 * case where
			 * to_search:
			 * 8 33 41 54
			 *          ^newpos
			 * intvl: 62-88
			 * newpos is smaller than the start, but the last occurence of the event; so event
			 * does not occur in intvl
			 */
			if (newpos + 1 >= to_search.size()) {
				return -1;
			}
			if (to_search[newpos+1]>= intvl.start ){
				/**
				 * case where
				 * to_search:
				 * 8 33 41 54
				 *       ^newpos
				 * intvl: 47-67
				 * newpos is smaller than the start, but the next occurence is within bounds
				 */
				if (to_search[newpos+1]<= intvl.end) {return to_search[newpos+1];}
				/**
				 * case where
				 * to_search:
				 * 8 33 41 54
				 *       ^newpos
				 * intvl: 47-51
				 * newpos is smaller than the start, but the next is not within bounds; event
				 * does not occur within intvl
				 */
				else {return -1;}
			}

			/**
			 * case where
			 * to_search:
			 * 8 33 41 54 67 88
			 *       ^newpos
			 *intvl: 63-100
			 *
			 */
			left = newpos + 1;
		}
		/**
		 * newpos is the start: this is the first occurrence
		 */
		else if (to_search[newpos] == intvl.start){
			return intvl.start;
		}
		else{
			if (newpos == 0) {
				/**
				 * case where
				 * to_search:
				 * 14 33 41 54
				 *  ^newpos
				 *  intvl: 8-17
				 *  newpos is larger than the start, but smaller than the end
				 */
				if (to_search[newpos]<= intvl.end) {return to_search[newpos];}
				/**
				 * case where
				 * to_search:
				 * 14 33 41 54
				 *  ^newpos
				 *  intvl: 8-12
				 *  newpos is larger than the start and the end, so not in range
				 */
				else {return -1;}
			}
			/**
			 * case where:
			 * to_search
			 * 8 33 47 54 67 88 91
			 * 			^newpos
			 * intvl: 7-69
			 * so the smaller interval on the right must contain the first occurrence
			 */
			right = newpos - 1;
		}
	}

}

long map_trace_checker::find_first_occurrence(const spot::ltl::multop* node, interval intvl){
	spot::ltl::multop::type opkind = node->op();
		switch(opkind){
		case spot::ltl::multop::Or:{
				int numkids = node->size();
				long total_first_occ = LONG_MAX;
				bool any_occ = false;
				for (int i =0; i<numkids ; i++){
					long first_occ =find_first_occurrence(node->nth(i), intvl);
					if ( first_occ != -1 && first_occ < total_first_occ){
						any_occ = true;
						total_first_occ = first_occ;
					}
				}
				if (any_occ == false) return -1;
				else return total_first_occ;
		}
		case spot::ltl::multop::And:{
			int numkids = node->size();
			long occurrence_loc = -1;
			for (int i =0; i<numkids ; i++){
				long first_occ =find_first_occurrence(node->nth(i), intvl);
				if ( first_occ == -1){
					return first_occ;
				} else if (occurrence_loc == -1){
					occurrence_loc = first_occ;
				} else if (occurrence_loc == first_occ){
					continue;
				} else {
					occurrence_loc = -1;
					break;
				}
			}
			return -1;

		}
		default:
			std::cerr<< "Unsupported Multop. Returning -1. \n";
			return -1;
		}
}

long map_trace_checker::find_first_occurrence(const spot::ltl::unop* node, interval intvl){

	spot::ltl::unop::type optype = node->op();

	switch (optype){

	//Ideally everything should be in negative normal form so this only runs on
	//individual events, in which case the recursion should not be too brutal.
	case spot::ltl::unop::Not:{
		long first_true = find_first_occurrence(node->child(),intvl);
		// if the expression in the not never occurs or first occurs
		// after the first element, then the first not occurence is
		// the start of the interval
		if (first_true == -1 || first_true > intvl.start){
			return intvl.start;
		}
		else if (intvl.start == intvl.end){
			return -1;
		}
		else {
			intvl.start++;
			return find_first_occurrence(node,intvl);
		}
	}
	default:
		std::cerr<< "Unsupported unary operation. Returning -1. \n";
		return -1;

	}

}

} /* namespace texada */
