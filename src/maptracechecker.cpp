/*
 * maptracechecker.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: clemieux
 */

#include "maptracechecker.h"
#include <ltlvisit/nenoform.hh>
#include <ltlvisit/tostring.hh>
#include <algorithm>
#include <deque>


//TODO: turns out I had the negative normal form wrong. See line 529.

namespace texada {

map_trace_checker::map_trace_checker(std::map<string_event,std::vector<long>> trace_map_):
	trace_map(trace_map_){

}

map_trace_checker::~map_trace_checker() {
	// TODO Auto-generated destructor stub
}

/**
 * Checks whether a formula holds on the trace inputed to the map trace checker.
 * @param node formula to check
 * @return true if node holds on the trace, false otherwise
 */
bool map_trace_checker::check_on_trace(const spot::ltl::formula* node){
	interval base_interval;
	std::vector<long> end_vector = trace_map.at(texada::string_event("EndOfTraceVar",true));
	base_interval.end = end_vector[0] -1;
	return check(node, base_interval);

}
/**
 * Checks whether a formula holds on the given interval. Switch method to assign
 * formula to its helper.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::check(const spot::ltl::formula* node, interval intvl){
	switch (node->kind()){
	case spot::ltl::formula::Constant:
		return check(static_cast<const spot::ltl::constant*>(node), intvl);
	case spot::ltl::formula::AtomicProp:
		return check(static_cast<const spot::ltl::atomic_prop*>(node),intvl);
	case spot::ltl::formula::UnOp:
		return check(static_cast<const spot::ltl::unop*>(node),intvl);
	case spot::ltl::formula::BinOp:
		return check(static_cast<const spot::ltl::binop*>(node),intvl);
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

/**
 * Checks whether a const formula holds on the given interval.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
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

/**
 * Checks whether a atomic proposition holds on the given interval.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::check(const spot::ltl::atomic_prop* node, interval intvl){
	long first_occurrence = find_first_occurrence(node, intvl);
	if (intvl.start == first_occurrence) return true;
	else return false;

}

/**
 * Checks whether a unop formula holds on the given interval.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::check(const spot::ltl::unop* node, interval intvl){
	spot::ltl::unop::type optype = node->op();

		switch (optype){

		// Globally case
		case spot::ltl::unop::G:{
			long first_occ = find_first_occurrence(spot::ltl::negative_normal_form(node->child(),true), intvl);
			if (first_occ == -1) return true;
			else return false;
		}
		// Finally case
		case spot::ltl::unop::F:{
			long first_occ = find_first_occurrence(node->child(), intvl);
			if (first_occ == -1) return false;
			else return true;
		}

		// Next case
		case spot::ltl::unop::X:{
			if (intvl.start == intvl.end){
				//TODO: something
				std::cerr<< "Something is wrong here \n";
				return false;
			}
			intvl.start++;
			return check(node->child(),intvl);
		}

		// Not case
		case spot::ltl::unop::Not:{
			return !(check(node->child(),intvl));
		}


		// Other operators are not LTL, don't support them
		default:
			std::cerr << "Unsupported unary operator. Returning false. \n" ;
			return false;

		}
}

/**
 * Checks whether a binop formula holds on the given interval.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::check(const spot::ltl::binop *node, interval intvl){
	spot::ltl::binop::type opkind = node->op();

	switch(opkind){

	//XOr case: if first is true, return true if second is false,
	//if first is false, return true if second is true.
	case spot::ltl::binop::Xor:
		return (check(node->first(),intvl))?
				!(check(node->second(),intvl)):
				check(node->second(),intvl);

	//Implies case: if first is true, return true if second is true,
	//if first is false, return true.
	case spot::ltl::binop::Implies:
		return check(node->first(),intvl)?
				check(node->second(),intvl):
				true;

	//Equiv case: if first is true, return true if second is true,
	//if first is false, return true if second is false.
	case spot::ltl::binop::Equiv:
		return check(node->first(),intvl)?
				check(node->second(),intvl):
				!check(node->second(),intvl);

	//Until case
	case spot::ltl::binop::U:{
		long first_occ_second = find_first_occurrence(node->second(),intvl);
		long first_occ_neg_first = find_first_occurrence(spot::ltl::negative_normal_form(node->first(),true), intvl);
		if (first_occ_neg_first < first_occ_second){
			return false;
		}
		else if (first_occ_second == -1){
			return false;
		}
		else return true;
	}

	//Release case
	case spot::ltl::binop::R:{
		long first_occ_neg_second = find_first_occurrence(spot::ltl::negative_normal_form(node->second(),true), intvl);
		long first_occ_first = find_first_occurrence(node->first(),intvl);
		if (first_occ_neg_second == -1){
			return true;
		}
		else if (first_occ_neg_second <= first_occ_first){
			return false;
		}
		else return true;
	}


	//Weak until case: identical to until except base case
	case spot::ltl::binop::W:{
		long first_occ_second = find_first_occurrence(node->second(),intvl);
		long first_occ_neg_first = find_first_occurrence(spot::ltl::negative_normal_form(node->first(),true), intvl);
		if (first_occ_neg_first < first_occ_second){
			return false;
		}
		else if (first_occ_neg_first !=-1 && first_occ_second == -1){
			return false;
		}
		else return true;
	}

	//Strong Release case: dual of weak until, identical to weak release except base
	case spot::ltl::binop::M:{
		long first_occ_neg_second = find_first_occurrence(spot::ltl::negative_normal_form(node->second(),true), intvl);
		long first_occ_first = find_first_occurrence(node->first(),intvl);
		if (first_occ_first == -1){
			return false;
		}
		else if (first_occ_neg_second == -1){
			return true;
		}
		else if (first_occ_neg_second <= first_occ_first){
			return false;
		}
		else return true;

	}

	default:
		std::cerr << "Unsupported binary operator. Returning false. \n";
		return false;

	}
}


/**
 * Checks whether a multop (and or or) formula holds on the given interval.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::check(const spot::ltl::multop* node, interval intvl){
	spot::ltl::multop::type opkind = node->op();

	switch(opkind){

	case spot::ltl::multop::Or:{
		int numkids = node->size();
		// for each of the multop's children, we check if it is true: if it is,
		// we short circuit and return true. If we have not returned by the
		// end of the loop, then none of the children were true and we return
		// false.
		for (int i =0; i<numkids ; i++){
			if (check(node->nth(i), intvl)){
				return true;
			}
		}
		return false;
	}
	case spot::ltl::multop::And:{
		int numkids = node->size();
		// for each of the multop's children, we check if it is false: if it is,
		// we short circuit and return false. If we have not returned by the
		// end of the loop, then none of the children were false and we return
		// true.
		for (int i =0; i<numkids ; i++){
			if (!check(node->nth(i), intvl)){
				return false;
			}
		}
		return true;
	}
	default:
		std::cerr << "Unsupported multiple operator. Returning false. \n";
		return false;

	}
}
/**
 * Finds the first occurrence of a formula in a given interval. Switch
 * method to helper functions
 * @param node formula to find
 * @param intvl interval to search in
 * @return first occurence of node in intvl, -1 if not found.
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::formula* node,interval intvl){
	switch (node->kind()){
		case spot::ltl::formula::Constant:
			return find_first_occurrence(static_cast<const spot::ltl::constant*>(node), intvl);
		case spot::ltl::formula::AtomicProp:
			return find_first_occurrence(static_cast<const spot::ltl::atomic_prop*>(node),intvl);
		case spot::ltl::formula::UnOp:
			return find_first_occurrence(static_cast<const spot::ltl::unop*>(node),intvl);
		case spot::ltl::formula::BinOp:
			return find_first_occurrence(static_cast<const spot::ltl::binop*>(node),intvl);
		case spot::ltl::formula::MultOp:
			return find_first_occurrence(static_cast<const spot::ltl::multop*>(node),intvl);
		case spot::ltl::formula::BUnOp:
			return find_first_occurrence(static_cast<const spot::ltl::bunop*>(node));
		case spot::ltl::formula::AutomatOp:
			return find_first_occurrence(static_cast<const spot::ltl::automatop*>(node));
		default:
			return -1;
		}
}


/**
 * Finds the first occurrence of an atomic proposition in a given interval
 * @param node atomic prop to find
 * @param intvl interval to search in
 * @return first occurrence position, -1 if not found.
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::atomic_prop* node,interval intvl){
	// REQUIRES: to_search is sorted. this should be assured earlier on.
	std::cout << "in find_first_occ, for " <<node->name() << " over: " <<
			intvl.start << "-" << intvl.end <<".\n";
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
			 * newpos is smaller than the start, but the last occurrence of the event; so event
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
				 * newpos is smaller than the start, but the next occurrence is within bounds
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

/**
 * Finds the first occurrence of a multop formula (and or or) in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return first occurrence of node in intvl
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::multop* node, interval intvl){
	spot::ltl::multop::type opkind = node->op();
		switch(opkind){

		// Or case: total_first_occ set to max. If any of the desired events
		// occur, they will happen before LONG_MAX, so the clause:
		// if(first_occ != -1 && first_occ < total_first_occ)
		// will be entered. This will set any_occ true so -1 will not be
		// returned. If it remains false we will correctly return -1 instead
		// of LONG_MAX.
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


		// And case: Originally developed the algorithm for find all occurrences
		// here, so now just get all occurrences and take the first one. Unlike
		// the or case, we can't short-circuit by simply finding the first
		// occurrences of each of the children: e.g. first occ of p&q&r?
		// (p) (j) (k&l) (r) (k) (a) (q&r) (a) (p) (p&q&r)
		// The first occurrence of the and has nothing to do with the first
		// occurrence of all the children.
		case spot::ltl::multop::And:{
			std::vector<long> common_occs = find_all_occurrence(node, intvl);
			if (common_occs.size() == 0) return -1;
			else return (*common_occs.begin());

		}
		default:
			std::cerr<< "Unsupported Multop. Returning -1. \n";
			return -1;
		}
}

/**
 * Finds the first occurrence of a unop formula in a given interval
 * @param node unop formula to find
 * @param intvl interval to search in
 * @return first occurrence of node in intvl, -1 if not found.
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::unop* node, interval intvl){

	spot::ltl::unop::type optype = node->op();

	switch (optype){

	//Ideally everything should be in negative normal form so this only runs on
	//individual events, in which case the recursion should not be too brutal.
	case spot::ltl::unop::Not:{
		std::cout << "Did we call not? \n";
		long first_true = find_first_occurrence(node->child(),intvl);
		// if the expression in the not never occurs or first occurs
		// after the first element, then the first not occurrence is
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

	// Find the first occurrence of the child, and return the event before
	// if it is in the interval. If it does not occur, return -1. If the
	// first occurrence is at the start, find the next first occurrence:
	// if there is none, return -1. Else return the event before that.
	case spot::ltl::unop::X:{
		long first_occ = find_first_occurrence(node->child(), intvl);
		if (first_occ == -1) return -1;
		else if (first_occ == intvl.start){
			if (intvl.start>=intvl.end) return -1;
			else {
				intvl.start++;
				long next_first_occ = find_first_occurrence(node->child(), intvl);
				if (next_first_occ == -1) return -1;
				else return --next_first_occ;
			}
		}
		else return --first_occ;
	}

	// Globally: find the last occurrence of the negation; first globally will
	// be after that.
	case spot::ltl::unop::G:{
		long last_neg_occ = find_last_occurrence(spot::ltl::negative_normal_form(node->child(),true),intvl);
		std::cout << "The last occurrence of "<< to_string(spot::ltl::negative_normal_form(node->child(),false)) << " was" << last_neg_occ << "\n";
		if (last_neg_occ == -1) return intvl.start;
		if (last_neg_occ == intvl.end) return -1;
		else return ++last_neg_occ;
	}

	case spot::ltl::unop::F:{
		long first_occ = find_first_occurrence(node->child(),intvl);
		if (first_occ == -1) return -1;
		else return intvl.start;
	}

	default:
		std::cerr<< "Unsupported unary operation. Returning -1. \n";
		return -1;

	}

}

/**
 * Finds the first occurrence of a binop in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return first occurrence of node in intvl
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::binop* node, interval intvl){
	spot::ltl::binop::type opkind = node->op();

	switch(opkind){

	//XOr case: Take the first of each one, unless they're at the same place.
	// Return the smallest of the first. If they're at the same place, we'll
	// need to find the next firsts. Worst case they always occur in a pair.
	case spot::ltl::binop::Xor:{
		long first_occ_first = find_first_occurrence(node->first(),intvl);
		long first_occ_second = find_first_occurrence(node->second(),intvl);
		if (first_occ_first == -1)return first_occ_second;
		else if (first_occ_second == -1) return first_occ_first;
		else if (first_occ_first < first_occ_second) return first_occ_first;
		else if (first_occ_first > first_occ_second) return first_occ_second;
		// at this point we know first and second are equal.
		else if (first_occ_second == intvl.end) return -1;
		else {
			intvl.start= first_occ_second + 1;
			return find_first_occurrence(node,intvl);
		}
	}

	// Find first of negation of first, and find first of validity of last.
	// return whichever is earlier.
	case spot::ltl::binop::Implies:{
		long first_occ_neg_first = find_first_occurrence(spot::ltl::negative_normal_form(node->first(),true), intvl);
		long first_occ_second = find_first_occurrence(node->second(),intvl);
		if (first_occ_neg_first == -1) return first_occ_second;
		else if (first_occ_second == -1) return first_occ_neg_first;
		else if (first_occ_neg_first < first_occ_second) return first_occ_neg_first;
		else return first_occ_second;

	}

	//Equiv case: return first where neither occur, of first where both occur.
	case spot::ltl::binop::Equiv:{
		long first_occ_first = find_first_occurrence(node->first(),intvl);
		long first_occ_second = find_first_occurrence(node->second(),intvl);
		if (first_occ_first != intvl.start && first_occ_second !=intvl.start){
			return intvl.start;
		}
		else if (first_occ_first == first_occ_second) {
			return first_occ_first;
		}
		else if (intvl.start >= intvl.end){
			return -1;
		}
		else{
			intvl.start++;
			return find_first_occurrence(node,intvl);
		}

	}

	// Until case: Find first occurrence of the second. Find the last negation
    // of the first strictly before the occurrence of the second. The first
	// until will be one after this last occurrence of the negation of the first.

	case spot::ltl::binop::U:{
		long first_occ_second = find_first_occurrence(node->second(),intvl);
		if (first_occ_second == -1) return -1;
		intvl.end = first_occ_second - 1;
		long last_occ_neg_first = find_last_occurrence(spot::ltl::negative_normal_form(node->first(),true),intvl);
		if (last_occ_neg_first == -1) return intvl.start;
		else return ++last_occ_neg_first;
	}

	// Release case: Find first occurrence of first. Find the last negation of the second
	// before (inclusive) the first occurrence of the first. First is either one
	// after this last occurrence or need to repeat with next first occurrence.
	// If there is no first occurrence of the first, return start if there is never an
	// occurrence of the last also.
	case spot::ltl::binop::R:{
		interval search_interval;
		long first_occ_first = find_first_occurrence(node->first(),intvl);
		long last_occ_neg_second;
		if (first_occ_first == -1){
			last_occ_neg_second = find_last_occurrence(spot::ltl::negative_normal_form(node->second(),true),intvl);
			if (last_occ_neg_second == -1) return intvl.start;
			else return -1;
		}
		search_interval.start = intvl.start;
		search_interval.end = first_occ_first;
		last_occ_neg_second = find_last_occurrence(spot::ltl::negative_normal_form(node->second(),true),search_interval);
		if (last_occ_neg_second == search_interval.end){
			intvl.start = ++search_interval.end;
			return find_first_occurrence(node, intvl);
		}
		else return ++last_occ_neg_second;
	}


	//Weak until case: identical to until except base case
	case spot::ltl::binop::W:{
		long first_occ_second = find_first_occurrence(node->second(),intvl);
		long last_occ_neg_first;
		if (first_occ_second == -1) {
			last_occ_neg_first = find_last_occurrence(spot::ltl::negative_normal_form(node->first(),true),intvl);
			if (last_occ_neg_first == -1){
				return intvl.start;
			} else return -1;
		}
		intvl.end = first_occ_second - 1;
		last_occ_neg_first = find_last_occurrence(spot::ltl::negative_normal_form(node->first(),true),intvl);
		if (last_occ_neg_first == -1) return intvl.start;
		else return ++last_occ_neg_first;
	}

	// Strong release: identical to release except base case:
	case spot::ltl::binop::M:{
		interval search_interval;
		long first_occ_first = find_first_occurrence(node->first(),intvl);
		long last_occ_neg_second;
		if (first_occ_first == -1) return -1;
		search_interval.start = intvl.start;
		search_interval.end = first_occ_first;
		last_occ_neg_second = find_last_occurrence(spot::ltl::negative_normal_form(node->second(),true),search_interval);
		if (last_occ_neg_second == search_interval.end){
			intvl.start = ++search_interval.end;
			return find_first_occurrence(node, intvl);
		}
		else return ++last_occ_neg_second;
	}

	default:
		std::cerr << "Unsupported binary operation. Returning -1. \n";
		return -1;
	}
}

/**
 * Finds the first occurrence of a constant in a given interval
 * @param node constant to find occurrence of
 * @param intvl interval to check in
 * @return first occurrence of node in intvl
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::constant *node, interval intvl){
	spot::ltl::constant::type value = node->val();
	switch (value){
	case spot::ltl::constant::True:
		return intvl.start;
	case spot::ltl::constant::False:
		return -1;
	case spot::ltl::constant::EmptyWord:
		std::cerr << "We came across the empty word. Returning -1. \n" ;
		return -1;
	default:
		return -1;
	}
}


/**
 * Finds the last occurrence of a formula in a given interval. Switch
 * method to helper functions.
 * @param node formula to find
 * @param intvl interval to search in
 * @return last occurence of node in intvl, -1 if not found.
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::formula* node, interval intvl){
	switch (node->kind()){
	case spot::ltl::formula::Constant:
		return find_last_occurrence(static_cast<const spot::ltl::constant*>(node), intvl);
	case spot::ltl::formula::AtomicProp:
		return find_last_occurrence(static_cast<const spot::ltl::atomic_prop*>(node),intvl);
	case spot::ltl::formula::UnOp:
		return find_last_occurrence(static_cast<const spot::ltl::unop*>(node),intvl);
	case spot::ltl::formula::BinOp:
		return find_last_occurrence(static_cast<const spot::ltl::binop*>(node),intvl);
	case spot::ltl::formula::MultOp:
		return find_last_occurrence(static_cast<const spot::ltl::multop*>(node),intvl);
	case spot::ltl::formula::BUnOp:
		return find_last_occurrence(static_cast<const spot::ltl::bunop*>(node));
	case spot::ltl::formula::AutomatOp:
		return find_last_occurrence(static_cast<const spot::ltl::automatop*>(node));
	default:
		return -1;
	}
}
/**
 * Finds the last occurrence of an atomic proposition in a given interval
 * @param node atomic prop to find
 * @param intvl interval to search in
 * @return last occurrence position, -1 if not found.
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::atomic_prop* node, interval intvl){
	// REQUIRES: to_search is sorted. this should be assured earlier on.
		std::cout << "in find_last_occ, for " <<node->name() << " over: " <<
				intvl.start << "-" << intvl.end <<".\n";
		std::vector<long> to_search = trace_map.at(string_event(node->name(),false));

		long left = 0;
		long right = to_search.size() - 1;
		long newpos;
		while (true){
			newpos = (right + left ) / 2;
			std::cout << newpos << "\n";
			if (to_search[newpos] > intvl.end){

				// case where all is to the left
				if (newpos == 0) {
					return -1;
				}
				//else if the previous is smaller than or equal to the end
				if (to_search[newpos-1]<= intvl.end ){
					// if it's not smaller than the start, return. Else we fail to find it
					if (to_search[newpos-1]>= intvl.start) {return to_search[newpos-1];}
					else {return -1;}
				}
				// if it's bigger than intvl.end and the one before was also too big,
				// we need to look at the left side
				right = newpos - 1;
			}

			else if (to_search[newpos] == intvl.end){
				return intvl.end;
			}
			// else to_search[newpos]<intvl.end
			else{
				std:: cout << "Do we get into the else \n";
				// if it's the last element
				if (newpos >= to_search.size() -1) {
					std::cout << "are we at the last element \n";
					// and it's larger than the start, then it's the last event
					if (to_search[newpos]>= intvl.start) {
						std::cout << "We're returning " << to_search[newpos] << "\n";
						return to_search[newpos];}
					// else if it's smaller than the start, it's not in the interval
					else {return -1;}
				}
				// else newpos is too small so we have to go to the right side
				left = newpos + 1;
			}
		}
}
/**
 * Finds the last occurrence of a multop formula (and or or) in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return last occurrence of node in intvl
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::multop* node, interval intvl){
	spot::ltl::multop::type opkind = node->op();
			switch(opkind){
			// Or case: total_last_occ set to max. If any of the desired events
			// occur, they will happen after -1, so the clause:
			// if( last_occ > total_last_occ)
			// will be entered, and the new last_occ will be set.
			case spot::ltl::multop::Or:{
					int numkids = node->size();
					long total_last_occ = -1;
					for (int i =0; i<numkids ; i++){
						long last_occ =find_last_occurrence(node->nth(i), intvl);
						if (last_occ > total_last_occ){
							total_last_occ = last_occ;
						}
					}
					return total_last_occ;
			}

			// And case: Again, just using the last of all occs.
			case spot::ltl::multop::And:{
				std::vector<long> common_occs = find_all_occurrence(node, intvl);
				if (common_occs.size() == 0) return -1;
				else return (*(common_occs.end()--));

			}
			default:
				std::cerr<< "Unsupported Multop. Returning -1. \n";
				return -1;
			}
}


/**
 * Finds the last occurrence of a unop formula in a given interval
 * @param node unop formula to find
 * @param intvl interval to search in
 * @return last occurrence of node in intvl, -1 if not found.
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::unop* node, interval intvl){
	spot::ltl::unop::type optype = node->op();

		switch (optype){

		// Not case: find the last occurrence of the child in the interval.
		// If the child is the last event and the interval is one long, then
		// we never have not child. If it's the last event in a longer intvl,
		// we can check for the second-last event by reducing the interval
		// length at the back.
		// Otherwise, if the last occurrence of the child is not the last
		// event, then the last event is the last negation.
		case spot::ltl::unop::Not:{
			long last_child = find_last_occurrence(node->child(),intvl);
			if (last_child == intvl.end){
				if (intvl.start == intvl.end) return -1;
				else{
					intvl.end--;
					return find_last_occurrence(node,intvl);
				}
			}
			else return intvl.end;
		}

		// Next case: find the last occurrence of the next (P). If P never
		// occurs, neither does XP. If the last occurrence of P is the first
		// event in an interval, then XP does not occur on that interval.
		// Any other last value is valid.
		case spot::ltl::unop::X:{
			long last_operand = find_last_occurrence(node->child(),intvl);
			if (last_operand == -1) return -1;
			if (last_operand ==  intvl.start) return -1;
			else return --last_operand;
		}

		// Globally case: find the last occurrence of the negation of the
		// child. If this last occurrence is the last event, we never
		// have G. In any other case, the last event is the last appearance of G.
		case spot::ltl::unop::G:{
			long last_neg_child = find_last_occurrence(spot::ltl::negative_normal_form(node->child(),true),intvl);
			if (last_neg_child == intvl.end) return -1;
			else return intvl.end;
		}

		// Finally case: the last occurrence of the child. This is the last
		// occurrence of "eventually".
		case spot::ltl::unop::F:
			return find_last_occurrence(node->child(),intvl);


		default:
			std::cerr << "Unsupported unary operator. Returning -1. \n";
			return -1;

		}
}

/**
 * Finds the last occurrence of a constant in a given interval
 * @param node constant to find occurrence of
 * @param intvl interval to check in
 * @return last occurrence of node in intvl
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::constant* node, interval intvl){
	spot::ltl::constant::type value = node->val();
	switch (value){
	case spot::ltl::constant::True:
		return intvl.end;
	case spot::ltl::constant::False:
		return -1;
	case spot::ltl::constant::EmptyWord:
		std::cerr << "We came across the empty word. Returning -1. \n" ;
		return -1;
	default:
		return -1;
	}
}
/**
 * Finds the last occurrence of a binop in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return last occurrence of node in intvl
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::binop* node, interval intvl){
	spot::ltl::binop::type opkind = node->op();

		switch(opkind){

		// Xor case: if both lasts are at the same location, we have a problem.
		// If they are at the same location in a one unit interval, XOR does not
		// occur. Else we know last_second is greater than the start of the
		// interval, so we can set the end of the interval to right
		// before the double occurrence correctly.
		// Else we know they do not occur at the same spot so we return the
		// one that occurs later.
		case spot::ltl::binop::Xor:{
			long last_first = find_last_occurrence(node->first(),intvl);
			long last_second = find_last_occurrence(node->second(),intvl);
			if (last_second == last_first){
				if (last_second == -1) return -1;
				if (last_second == intvl.start) return -1;
				else {
					intvl.end = last_second -1;
					return find_last_occurrence(node,intvl);
				}
			}
			if (last_second > last_first) return last_second;
			else return last_first;
		}

		// Return the last negation of the first or occurrence of the last.
		// If they are both equal to -1, then the last else will be entered
		// and -1 will be returned.
		case spot::ltl::binop::Implies:{
			long last_neg_first = find_last_occurrence(spot::ltl::negative_normal_form(node->first(),true),intvl);
			long last_second = find_last_occurrence(node->second(),intvl);
			if (last_neg_first >last_second) return last_neg_first;
			else return last_second;
		}

		// Equiv case: if both are equal, then the last occurrence is at
		// the end (we know that at the end, there is not only one of
		// first or second occurring)
		// Else we just step by 1 because the event before the last could
		// have neither the first nor the second, thus be an occurrence
		// of equiv
		case::spot::ltl::binop::Equiv:{
			long last_first = find_last_occurrence(node->first(),intvl);
			long last_second = find_last_occurrence(node->second(),intvl);
			if (last_first == last_second) return intvl.end;
			else if (last_first == intvl.end || last_second == intvl.end){
				if (intvl.end == intvl.start) return -1;
				else {
					intvl.end--;
					return find_last_occurrence(node,intvl);
				}

			}
			else return intvl.end;

		}

		// Note that p U q -> <>q, so we need only find the last occurrence
		// of q to find the last occurrence of p U q. (And if it never occurs,
		// neither does the until.
		case::spot::ltl::binop::U:{
			return find_last_occurrence(node->second(),intvl);
		}

		// Similar to until, but if the last event does not violate the first,
		// then the last event is the last occurrence.
		case::spot::ltl::binop::W:{
			interval temp;
			temp.start = intvl.end;
			temp.end = intvl.end;
			if (check(node->first(), temp)){
				return intvl.end;
			}
			else return find_last_occurrence(node->second(),intvl);
		}

		// Release case: q R p
		// if the last is p, then that is the last occurrence.
		// else we must find the last occurrence of q -- if it
		// does not occur then we return -1. Id p holds at that
		// last occurrence, then that is the last occurrence
		// of the release -- else we need to check for the
		// next las occurrence.
		case::spot::ltl::binop::R:{
			interval temp;
			temp.start = intvl.end;
			temp.end = intvl.end;
			if (check(node->second(), temp)){
				return temp.end;
			}
			long last_occ_first = find_last_occurrence(node->first(),intvl);
			if (last_occ_first == -1) return -1;
			temp.start = last_occ_first;
			temp.end = last_occ_first;
			if (check (node->second(),temp)){
				return temp.end;
			}
			else if (last_occ_first == intvl.start) return -1;
			else {
				intvl.end = last_occ_first - 1;
				return find_last_occurrence(node,intvl);
			}

		}

		// Same as weak release, but we don't consider whether the last
		// element is p, as that is not valid for strong release.
		case::spot::ltl::binop::M:{
			interval temp;
			long last_occ_first = find_last_occurrence(node->first(),intvl);
			if (last_occ_first == -1) return -1;
			temp.start = last_occ_first;
			temp.end = last_occ_first;
			if (check (node->second(),temp)){
				return temp.end;
			}
			else if (last_occ_first == intvl.start) return -1;
			else {
				intvl.end = last_occ_first - 1;
				return find_last_occurrence(node,intvl);
			}
		}

		}
	return -1;
}

//TODO: MAKE SURE ALL THESE ARE IN SORTED ORDER PLEASE
/**
 * Finds all occurrences of a formula in a given interval. Switch
 * method to helper functions.
 * @param node formula to find
 * @param intvl interval to search in
 * @return all occurences of node in intvl, empty vector if none
 */
std::vector<long> map_trace_checker::find_all_occurrence(const spot::ltl::formula* node, interval intvl){
	switch (node->kind()){
	case spot::ltl::formula::Constant:
		return find_all_occurrence(static_cast<const spot::ltl::constant*>(node), intvl);
	case spot::ltl::formula::AtomicProp:
		return find_all_occurrence(static_cast<const spot::ltl::atomic_prop*>(node),intvl);
	case spot::ltl::formula::UnOp:
		return find_all_occurrence(static_cast<const spot::ltl::unop*>(node),intvl);
	case spot::ltl::formula::BinOp:
		return find_all_occurrence(static_cast<const spot::ltl::binop*>(node),intvl);
	case spot::ltl::formula::MultOp:
		return find_all_occurrence(static_cast<const spot::ltl::multop*>(node),intvl);
	case spot::ltl::formula::BUnOp:
		return find_all_occurrence(static_cast<const spot::ltl::bunop*>(node));
	case spot::ltl::formula::AutomatOp:
		return find_all_occurrence(static_cast<const spot::ltl::automatop*>(node));
	default:
		std::vector<long> blank_vec;
		return blank_vec;
	}

}
/**
 * Finds all occurrences of an atomic proposition in a given interval
 * @param node atomic prop to find
 * @param intvl interval to search in
 * @return all occurrences of node in intvl
 */
std::vector<long> map_trace_checker::find_all_occurrence(const spot::ltl::atomic_prop* node, interval intvl){
	//TODO:FINISH... make sure bounds are correct and stuff....
	std::cout << "In find all occurrence \n";
	std::vector<long> to_search = trace_map.at(string_event(node->name(),false));
	long first_element = find_first_occurrence(node, intvl);
	long last_element = find_last_occurrence(node,intvl);
	std::vector<long> ret_vec;
	int left = 0;
	int right = to_search.size()-1;
	int probe = (left+right)/2;
	std::cout << "probe: " << probe << ".\n";
	while (true){
		std::cout << "In the loop \n";
		probe = (left+right)/2;
		std::cout << "probe: " << probe << ".\n";
		if (to_search.at(probe)==first_element){
			std::cout << "In the first if \n";
			break;
		}
		if (to_search.at(probe)<first_element){
			std::cout << "In the second if \n";
			left = probe + 1;
			continue;
		}
		if (to_search.at(probe)>first_element){
			std::cout << "In the third if \n";
			right = probe - 1;
			continue;
		}
	}
	for (int i=0; i < to_search.size(); i++){
		std::cout << "probe: " << probe + i << ".\n";
		if (to_search.at(probe + i)> last_element) break;
		ret_vec.push_back(to_search.at(probe+i));
	}

	return ret_vec;
}
/**
 * Finds all of a multop formula (and or or) in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return all occurrences of node in intvl
 */
std::vector<long> map_trace_checker::find_all_occurrence(const spot::ltl::multop* node, interval intvl){
	spot::ltl::multop::type opkind = node->op();
	std::cout << "Do we get into multop find all \n";
	std::vector<long> blank_vec;
		switch(opkind){
		// Or case: for each child, find all occurrences, and insert into a set
		// of all occurrences, which will be returned.
		case spot::ltl::multop::Or:{
			int numkids = node->size();
			std::vector<long> child_occs;
			std::set<long> common_occs;
			for (int i =0; i<numkids ; i++){
				child_occs = find_all_occurrence(node->nth(i),intvl);
				common_occs.insert(child_occs.begin(),child_occs.end());
				}
			return std::vector<long> (common_occs.begin(), common_occs.end());
		}
		// And case: Find all the occurrences of the first child and put this into
		// common occurrences. For each subsequent child, find all its occurrences
		// (ordered.) Then for each common occurrence, search for it in the new
		// child's occurrences; if it does not occur, erase it. Continue until there
		// are no common occurrences or until we have gone through all the children.
		case spot::ltl::multop::And:{
			int numkids = node->size();
			std::vector<long> child_occs = find_all_occurrence(node->nth(0),intvl);
			std::list<long> common_occs (child_occs.begin(), child_occs.end());
			for (int i =1; i<numkids ; i++){
				std::cout << "Do we enter the and loop \n";
				child_occs = find_all_occurrence(node->nth(i),intvl);
				for(std::list<long>::iterator it = common_occs.begin(); it != common_occs.end(); it++){
					if (!(std::binary_search(child_occs.begin(),child_occs.end(),*it))){
						common_occs.erase(it);
					}
				}
				if (common_occs.size() == 0) break;
			}
			return std::vector<long>(common_occs.begin(),common_occs.end());

		}
		default:
			std::cerr<< "Unsupported Multop. Returning empty vector. \n";
			return blank_vec;
		}


}

/**
 * Finds all occurrences of a unop formula in a given interval
 * @param node unop formula to find
 * @param intvl interval to search in
 * @return all occurrences of node in intvl
 */
std::vector<long> map_trace_checker::find_all_occurrence(const spot::ltl::unop* node, interval intvl){
	spot::ltl::unop::type optype = node->op();
	std::vector<long> return_vec;

		switch (optype){

		// Again, as negatives are usually pushed in as negative normal form,
		// this shouldn't be too awful...
		// Not case: find all the nots, and not those.
		// i begins at the start. We increment it by one and add it to return_vec
		// until we reach an event in not_occs. At this point we set i to one
		// beyond the event, and find the next event. Then we repeat the process.
		// When we exit the loop, i will be one beyond the last event, but
		// those occurrences of not i will not have been added, so we must
		// do that after
		case spot::ltl::unop::Not:{
			std::vector<long> not_occs = find_all_occurrence(node->child(),intvl);
			int i = intvl.start;
			for(std::vector<long>::iterator it = not_occs.begin();
					it != not_occs.end(); it++){
				while (i<*it){
					return_vec.push_back(i);
					i++;
				}
				i = *it+1;
			}
			while (i<=intvl.end){
				return_vec.push_back(i);
				i++;
			}
			return return_vec;
		}

		// Find all occurrences of the child, one minus those. Unless it's the
		// first event, in which case we skip it.
		// return_vec will be ordered because find_all_occurrence should return
		// an ordered vector.
		case::spot::ltl::unop::X:{
			std::vector<long> child_occs = find_all_occurrence(node->child(),intvl);
			for (std::vector<long>::iterator it = child_occs.begin();
					it != child_occs.end(); it++){
				if (*it == intvl.start) continue;
				return_vec.push_back(--*it);
			}
			return return_vec;
		}

		// Globally: By definition of G (P) as P holding on all suffix traces,
		// all occurrences of globally will simply be from the first to the end.
		// RETURN_VEC will be ordered because each subsequent i inserted
		// is one larger than the one preceding it.
		case spot::ltl::unop::G:{
			std::cout<< "Finding all occurrence of the thing \n";
			long first_occ = find_first_occurrence(node, intvl);
			std::cout << "The first occurrence was..." << first_occ << "\n";
			if (first_occ == -1) return return_vec;
			else{
				for (int i = first_occ; i<=intvl.end; i++){
					std::cout << "Pushing back " << i <<"\n";
					return_vec.push_back(i);
				}
				return return_vec;
			}

		}

		// Eventually: Eventually holds until the last occurrence of its child,
		// i.e. its last occurrence.
		// RETURN_VEC will be ordered because each subsequent i inserted
		// is one larger than the one preceding it.
		case spot::ltl::unop::F:{
			long last_occ = find_last_occurrence(node,intvl);
			if (last_occ == -1) return return_vec;
			else{
				for (int i =intvl.start; i<=last_occ; i++){
					return_vec.push_back(i);
				}
				return return_vec;
			}
		}

		default:
			std::cerr << "Unsupported unop. Returning empty vector. \n";
			return return_vec;
		}

}

/**
 * Finds all occurrences of a constant in a given interval
 * @param node constant to find occurrence of
 * @param intvl interval to check in
 * @return all occurrences of node in intvl
 */
std::vector<long> map_trace_checker::find_all_occurrence(const spot::ltl::constant* node, interval intvl){
	std::vector<long> blank_vec;
	spot::ltl::constant::type value = node->val();

	switch (value){
	case spot::ltl::constant::True:{
		for (int i = intvl.start; i<=intvl.end; i++){
			blank_vec.push_back(i);
		}
		return blank_vec;
	}
	case spot::ltl::constant::False:
		return blank_vec;
	case spot::ltl::constant::EmptyWord:
		std::cerr << "We came across the empty word. Returning empty vector \n" ;
		return blank_vec;
	default:
		return blank_vec;
	}

}
/**
 * Finds all occurrences of a binop in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return all occurrences of node in intvl
 */
std::vector<long> map_trace_checker::find_all_occurrence(const spot::ltl::binop* node, interval intvl){
	std::cout <<"Entered unfinished method \n";
	spot::ltl::binop::type opkind = node->op();
	std::vector<long> return_vec;

		switch(opkind){

		//Complicated, finish later
		case spot::ltl::binop::Xor:{/*
			std::vector<long> first_vec = find_all_occurrence(node->first(),intvl);
			std::vector<long> second_vec = find_all_occurrence(node->second(),intvl);

			//making sure neither are empty
			if (first_vec.empty()) return second_vec;
			else if (second_vec.empty()) return first_vec;

			std::deque<long> first_q = std::deque<long>(first_vec.begin(),first_vec.end());
			std::deque<long> second_q = std::deque<long>(second_vec.begin(),second_vec.end());

			// setting these both to -1 will get them to be set correctly
			// on first run through the loop....
			long next_first = -1;
			long next_second = -1;


			while (!first_q.empty() && !second_q.empty()){
				if (next_first == next_second){
					next_first = first_q.pop_front();
					next_second = second_q.pop_front();
				}
				else if (next_first < next_second){
					return_vec.push_back(next_first);
					next_first = first_q.pop_front();
				}
				else {
					return_vec.push_back(next_second);
					next_second = second_q.pop_front();
				}
			}

			if (first_q.empty()){
				if (next_first == next_second){

				}
			}


		*/}

		case::spot::ltl::binop::Implies:{

		}

		return return_vec;
		}


	}
} /* namespace texada */
