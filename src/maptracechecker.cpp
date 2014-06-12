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

/**
 * Checks whether a formula holds on the trace inputed to the map trace checker.
 * @param node formula to check
 * @return true if node holds on the trace, false otherwise
 */
bool map_trace_checker::check_on_trace(const spot::ltl::formula* node){
	interval base_interval;
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
 * Finds the first occurrence of an atomic proposition in a given interval
 * @param node atomic prop to find
 * @param intvl interval to search in
 * @return first occurrence position, -1 if not found.
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
		// returned.
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

		// And case:
		// TODO: THIS IS WRONG. FIX THIS
		case spot::ltl::multop::And:{
			int numkids = node->size();
			long total_max_occurrence = -1;
			// if any of the events do not occur, we short-circuit
			// in the first if statement and return -1: else we find
			// the last first occurrence, since this is the only possible
			// place where all events can occur
			for (int i =0; i<numkids ; i++){
				long first_occ =find_first_occurrence(node->nth(i), intvl);
				if ( first_occ == -1){
					return first_occ;
				} else if (first_occ > total_max_occurrence){
					total_max_occurrence = first_occ;
				}
			}
			// now we check if all the events hold at the possible
			// first occurrence; if they do not, return false
			return -1;

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
	//TODO:FINISH
	return -1;
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
	//TODO:FINISH
	return -1;
}
/**
 * Finds the last occurrence of an atomic proposition in a given interval
 * @param node atomic prop to find
 * @param intvl interval to search in
 * @return last occurrence position, -1 if not found.
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::atomic_prop* node, interval intvl){
	//TODO:FINISH
	return -1;
}
/**
 * Finds the last occurrence of a multop formula (and or or) in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return last occurrence of node in intvl
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::multop* node, interval intvl){
	//TODO:FINISH
	return -1;
}

/**
 * Finds the first occurrence of a unop formula in a given interval
 * @param node unop formula to find
 * @param intvl interval to search in
 * @return first occurrence of node in intvl, -1 if not found.
 */
long map_trace_checker::find_last_occurrence(const spot::ltl::unop* node, interval intvl){
	//TODO:FINISH
	return -1;
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
	//TODO:FINISH
	return -1;
}


/**
 * Finds all occurrences of a formula in a given interval. Switch
 * method to helper functions.
 * @param node formula to find
 * @param intvl interval to search in
 * @return all occurences of node in intvl, empty vector if none
 */
std::vector<long> map_trace_checker::find_all_occurrence(const spot::ltl::formula* node, interval intvl){
	//TODO:FINISH
	std::vector<long> blank_vec;
	return blank_vec;
}
/**
 * Finds all occurrences of an atomic proposition in a given interval
 * @param node atomic prop to find
 * @param intvl interval to search in
 * @return all occurrences of node in intvl
 */
std::vector<long> map_trace_checker::find_all_occurrence(const spot::ltl::atomic_prop* node, interval intvl){
	//TODO:FINISH
	std::vector<long> blank_vec;
	return blank_vec;
}
/**
 * Finds all of a multop formula (and or or) in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return all occurrences of node in intvl
 */
std::vector<long> map_trace_checker::find_all_occurrence(const spot::ltl::multop* node, interval intvl){
	//TODO:FINISH
	std::vector<long> blank_vec;
	return blank_vec;
}

/**
 * Finds all occurrences of a unop formula in a given interval
 * @param node unop formula to find
 * @param intvl interval to search in
 * @return all occurrences of node in intvl
 */
std::vector<long> map_trace_checker::find_all_occurrence(const spot::ltl::unop* node, interval intvl){
	//TODO:FINISH
	std::vector<long> blank_vec;
	return blank_vec;
}

/**
 * Finds all occurrences of a constant in a given interval
 * @param node constant to find occurrence of
 * @param intvl interval to check in
 * @return all occurrences of node in intvl
 */
std::vector<long> map_trace_checker::find_all_occurrence(const spot::ltl::constant* node, interval intvl){
	//TODO:FINISH
	std::vector<long> blank_vec;
	return blank_vec;
}
/**
 * Finds all occurrences of a binop in a given interval
 * @param node formula to find occurrence of
 * @param intvl interval to check in
 * @return all occurrences of node in intvl
 */
std::vector<long> map_trace_checker::find_all_occurrence(const spot::ltl::binop* node, interval intvl){
	//TODO:FINISH
	std::vector<long> blank_vec;
	return blank_vec;
}
} /* namespace texada */
