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
	//std::cout << "###";
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
	//std::cout << "Checking " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
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
	//std::cout << "Checking " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
	try{
	std::vector<long> to_search = trace_map.at(string_event(node->name(),false));
	if (std::binary_search(to_search.begin(),to_search.end(),intvl.start)){
		return true;
	}
	else return false;
	}catch (std::out_of_range &e){
		return false;
	}

}

/**
 * Checks whether a unop formula holds on the given interval.
 * @param node formula to check
 * @param intvl interval to check on
 * @return true if node holds over intvl, false otherwise
 */
bool map_trace_checker::check(const spot::ltl::unop* node, interval intvl){
	//std::cout << "Checking " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
	spot::ltl::unop::type optype = node->op();

		switch (optype){

		// Globally case
		case spot::ltl::unop::G:{
			long first_occ = find_first_occurrence(spot::ltl::negative_normal_form(node->child(),true), intvl);
			//std::cout << "First occ of " << spot::ltl::to_string(spot::ltl::negative_normal_form(node->child(),true)) << " over "<<
			//		intvl.start << "-" << intvl.end << " is " << first_occ << ". \n";
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
				//TODO: tests for this
				std::vector<long> lastevent = trace_map.at(string_event("EndOfTraceVar",true));
				intvl.start=lastevent[0];
				intvl.end=lastevent[0];
				return check(node->child(),intvl);
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
	//std::cout << "Checking " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
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
		// if the first never occurs but a negation of the second does, this is incorrect?
		else if (first_occ_first == -1){
			return false;
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
	//std::cout << "Checking " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
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
	//std::cout << "Finding first occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
	// REQUIRES: to_search is sorted. this should be assured earlier on.
	try{
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
				//std::cout << "It's -1. \n";
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
				if (to_search[newpos+1]<= intvl.end) {
					//std::cout << "It's " << to_search[newpos+1] << ".\n";
					return to_search[newpos+1];}
				/**
				 * case where
				 * to_search:
				 * 8 33 41 54
				 *       ^newpos
				 * intvl: 47-51
				 * newpos is smaller than the start, but the next is not within bounds; event
				 * does not occur within intvl
				 */
				else {
					//std::cout << "It's -1. \n";
					return -1;}
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
			//std::cout << "It's " << intvl.start << ".\n";
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
				if (to_search[newpos]<= intvl.end) {
					//std::cout << "It's " << to_search[newpos] << ".\n";
					return to_search[newpos];}
				/**
				 * case where
				 * to_search:
				 * 14 33 41 54
				 *  ^newpos
				 *  intvl: 8-12
				 *  newpos is larger than the start and the end, so not in range
				 */
				else {
					//std::cout << "It's -1.\n";
					return -1;}
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
	} catch (std::out_of_range &e){
		// this means we didn't find the event in the trace map, so it never occurs;
		//std::cout << "It's -1.\n";
		return -1;
	}
}

/**
 * Finds the first occurrence of a multop formula (and or or) in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return first occurrence of node in intvl
 */
long map_trace_checker::find_first_occurrence(const spot::ltl::multop* node, interval intvl){
	//std::cout << "Finding first occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
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
						//std::cout << "##First occ of " << spot::ltl::to_string(node->nth(i)) << " over " <<
						//		intvl.start << "-" << intvl.end << " is " << first_occ << ". \n";
						total_first_occ = first_occ;
					}
				}
				if (any_occ == false) return -1;
				else return total_first_occ;
		}



		// We find the first occurrence of each child; if one does not occur,
		// the and never occurs.
		// else we take the last of these positions and check if the and occurs
		// there. If it does not, we go one past and check for the first on that
		// interval.
		case spot::ltl::multop::And:{
			int numkids = node->size();
			long total_first_occ = -1;
			for (int i = 0; i<numkids;i++){
				long first_occ = find_first_occurrence(node->nth(i),intvl);
				if (first_occ == -1) return -1;
				if (first_occ > total_first_occ){
					total_first_occ = first_occ;
				}
			}
			intvl.start=total_first_occ;
			for (int i = 0; i<numkids;i++){
				if (!check(node->nth(i),intvl)) {
					if (intvl.start != intvl.end){
						intvl.start++;
						return find_first_occurrence(node,intvl);
					}
					else return -1;
				}
			}
			return total_first_occ;
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
	//std::cout << "Finding first occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
	spot::ltl::unop::type optype = node->op();

	switch (optype){

	//Ideally everything should be in negative normal form so this only runs on
	//individual events, in which case the recursion should not be too brutal.
	case spot::ltl::unop::Not:{
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

	// Find the first occurrence of the child in the interval of nexts.
	// Return the corresponding event that comes before it. Or -1 if
	// it does not.
	case spot::ltl::unop::X:{
		intvl.start++;
		intvl.end++;
		long first_occ = find_first_occurrence(node->child(), intvl);
		if (first_occ == -1) return -1;
		else {
			//std::cout << "#$$Returning first occ of " << spot::ltl::to_string(node) << " is " << first_occ -1 << ".\n";
			return first_occ -1;
		}
	}

	// Globally: find the last occurrence of the negation; first globally will
	// be after that.
	case spot::ltl::unop::G:{
		long last_neg_occ = find_last_occurrence(spot::ltl::negative_normal_form(node->child(),true),intvl);
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
	//std::cout << "Finding first occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
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
		//std::cout << "First occurrence of the first was " << first_occ_first << "\n";
		long last_occ_neg_second;
		if (first_occ_first == -1){
			last_occ_neg_second = find_last_occurrence(spot::ltl::negative_normal_form(node->second(),true),intvl);
			if (last_occ_neg_second == -1) return intvl.start;
			else return -1;
		}
		search_interval.start = intvl.start;
		search_interval.end = first_occ_first;
		last_occ_neg_second = find_last_occurrence(spot::ltl::negative_normal_form(node->second(),true),search_interval);
		//std::cout << "Last occurrence of neg second was " << last_occ_neg_second << ": " << intvl.start << "-"<<search_interval.end << "\n";
		if (last_occ_neg_second == search_interval.end){
			if (intvl.end == search_interval.end) return -1;
			intvl.start = ++search_interval.end;
			return find_first_occurrence(node, intvl);
		}
		//the following broke some things and fixed others TODO address this.
		else if (last_occ_neg_second == -1){
			return intvl.start;
		}
		else {
			return ++last_occ_neg_second;
		}
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
		//std::cout << "First occurrence of the first was " << first_occ_first << "\n";
		long last_occ_neg_second;
		if (first_occ_first == -1) return -1;
		search_interval.start = intvl.start;
		search_interval.end = first_occ_first;
		last_occ_neg_second = find_last_occurrence(spot::ltl::negative_normal_form(node->second(),true),search_interval);
		//std::cout << "Last occurrence of neg second was " << last_occ_neg_second << ": " << intvl.start << "-"<<search_interval.end << "\n";
		//std::cout << "Are we entering the if? " << (last_occ_neg_second == search_interval.end) << ".\n";
		if (last_occ_neg_second == search_interval.end){
			if (intvl.end == search_interval.end) return -1;
			intvl.start = ++search_interval.end;
			return find_first_occurrence(node, intvl);
		}
		if (last_occ_neg_second == -1){
			return intvl.start;
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
	//std::cout << "Finding first occurrence of " << spot::ltl::to_string(node) << " on trace from " << intvl.start << "-" << intvl.end<< "\n";
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
	try {
		std::vector<long> to_search = trace_map.at(string_event(node->name(),false));

		long left = 0;
		long right = to_search.size() - 1;
		long newpos;
		while (true){
			newpos = (right + left ) / 2;
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
				// if it's the last element
				if (newpos >= to_search.size() -1) {
					// and it's larger than the start, then it's the last event
					if (to_search[newpos]>= intvl.start) {
						return to_search[newpos];}
					// else if it's smaller than the start, it's not in the interval
					else {return -1;}
				}
				// else newpos is too small so we have to go to the right side
				left = newpos + 1;
			}
		}
	}catch (std::out_of_range &e){
		// this means we didn't find the event in the trace map, so it never occurs;
		return -1;
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
				int numkids = node->size();
				long total_last_occ = LONG_MAX;
				for (int i = 0; i<numkids;i++){
					long last_occ = find_last_occurrence(node->nth(i),intvl);
					if (last_occ == -1) return -1;
					if (last_occ < total_last_occ){
						total_last_occ = last_occ;
					}
				}
				interval temp;
				temp.start = total_last_occ;
				for (int i = 0; i<numkids;i++){
					if (!check(node->nth(i),temp)) {
						if (intvl.start != temp.start){
							intvl.end = total_last_occ - 1;
							return find_last_occurrence(node,intvl);
						}
						else return -1;
					}
				}
			return total_last_occ;
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
		// occurs, neither does XP. Else return one before the last occurrence,
		// which will be in the original interval.
		case spot::ltl::unop::X:{
			intvl.start++;
			intvl.end++;
			long last_operand = find_last_occurrence(node->child(),intvl);
			if (last_operand == -1) return -1;
			else return last_operand-1;
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

} /* namespace texada */
