/*
 * orderdecider.cpp
 *
 *  Created on: May 6, 2015
 *      Author: clemieux
 */

#include "orderdecider.h"
#include "mostoccurringsubsetgatherer.h"

#include <iostream>

namespace texada {

void print_set(set<string> set){
    for (std::set<std::string>::iterator it = set.begin(); it != set.end(); it++){
        std::cout << *it << " ";
    }
    std::cout << "\n";
}

void print_map(map<set<string>,int> map){
   // std::cout << "I'm printing a map!\n";
    for (std::map<std::set<std::string>,int>::iterator it = map.begin(); it != map.end(); it++){
        print_set(it->first);
        std::cout << " => " << it->second << "\n";
    }
}

/**
 * creates an order decider
 * @param count_map
 * @param n
 */
order_decider::order_decider(map<set<string>,int> count_map, int n) {
    original_count_map = count_map;
    total_size = n;

}

order_decider::~order_decider() {
    // TODO Auto-generated destructor stub
}


/**
 * Public wrapper for determine_order with parameter
 */
void order_decider::determine_order(){
    determine_order(original_count_map);
}

/**
 * Fills vars_in_order in desired order for ideal memoization tactics
 */
void order_decider::determine_order(map<set<string>, int> input_map){
  //  std::cout << "Welcome to wonderful magic land, map on input:\n";
    map<set<string>, int>::iterator it = input_map.begin();
  //  print_map(input_map);
    std::pair<int,int> max(0,0);
    set<string> start_set;
    while ( it != input_map.end()){
        std::pair<int,int> cur (it->second,it->first.size());
        if ((cur.first > max.first) ||((cur.first == max.first)&& (cur.second > max.second))){
            start_set = it->first;
            max = cur;
        }
        it++;
    }
  //  print_set(start_set);
    insert_in_order(start_set);
    if (vars_so_far.size() == total_size){
        return;
    }
    // if there are so vars remaining to order, do the same process
    // reduced map
    it = input_map.begin();
    while ( it != input_map.end()){
        set<string> current_set = it->first;
     //   std::cout << "Current set: ";
        int current_count = it->second;
      //  print_set(current_set);
        std::set<string>::iterator it2 = current_set.begin();
        while (it2 != current_set.end()){
            if (vars_so_far.count(*it2)){
           //     std::cout << "Going to delete " << *it2 << "\n";
                std::set<string>::iterator to_delete = it2;
                it2++;
                current_set.erase(to_delete);
            }
            else
                it2++;
        }
        map<set<string>, int>::iterator to_delete = it;
        it++;
        input_map.erase(to_delete);

        if (current_set.size() != 0){
            if (input_map.find(current_set)!= input_map.end()){
                input_map.find(current_set)->second += current_count;
            }
            else {
                input_map.emplace(current_set, current_count);
            }
        }

        //std::cout << "Map after set: \n";
       // print_map(input_map);
    }
  //  std::cout<< "Map on output:\n";
  //  print_map(input_map);
    determine_order(input_map);

}



/**
 * Inserts the contents of to_insert into vars_in_order in the way that
 * makes most sense for memoization (i.e. tracking the largest number of
 * occurrences in subtrees)
 * @param to_insert
 */
void order_decider::insert_in_order(set<string> to_insert){
    if (to_insert.size() == 0){
        return;
    }
    else {
        string inserter = most_occurring_with_set(to_insert, vars_so_far);
        vars_so_far.insert(inserter);
        vars_in_order.push_back(inserter);
        to_insert.erase(inserter);
    }
    insert_in_order(to_insert);
}

/**
 * Decides which string in occurrers occurs most often with occurrees in
 * original_count_map. Assumes occurrers and occurrees are non-empty. Splits
 * ties in inverse order in the set.
 * @param occurrers
 * @param occurrees
 * @return
 */
string order_decider::most_occurring_with_set(set<string> occurrers, set<string> occurrees){
    int max = 0;
    string best;
    for (set<string>::iterator it = occurrers.begin(); it != occurrers.end(); it++){
        int count = count_occurrences_with_set(*it, occurrees);
        if (count >= max){
            best = *it;
            max = count;
        }
    }
    return best;
}

/**
 * Counts the number of times occurrer occurs with occurrees in the sets that are
 * the keys in original_count_map
 * @param occurrer
 * @param occurrees
 * @return
 */
int order_decider::count_occurrences_with_set(string occurrer, set<string> occurrees){
    int count = 0;
    set<string> all = occurrees;
    all.insert(occurrer);
    for( map<set<string>, int>::iterator it = original_count_map.begin(); it != original_count_map.end();
            it++){
        set<string> search_set = it->first;
        set<string>::iterator allit = all.begin();
        // while we still have things to check occur, and those things occur
        while ((allit != all.end()) && (search_set.count(*allit))){
            allit++;
        }
        // if the count was 1 for all elements in all, all is a subset
        // of search set, so increment count.
        if (allit == all.end()){
           count += it->second;
        }

    }
    return count;
}

/**
 * Returns the memoization-optimized order for f (first element of returned
 * vector should be the one that spans the most time).
 * @param f formula to find op
 * @return
 */
vector<string> optimize_var_order(const spot::ltl::formula * f, int n){
    most_occurring_subset_gatherer gatherer = most_occurring_subset_gatherer();
    f->accept(gatherer);
    order_decider decider = order_decider(gatherer.set_to_count_map, n);
    decider.determine_order();
    return decider.vars_in_order;
}

} /* namespace texada */
