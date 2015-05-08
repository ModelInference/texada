/*
 * orderdecider.h
 *
 *  Created on: May 6, 2015
 *      Author: clemieux
 */

#ifndef ORDERDECIDER_H_
#define ORDERDECIDER_H_

#include <set>
#include <string>
#include <map>
#include <vector>

#include <ltlast/allnodes.hh>

namespace texada {
using std::set;
using std::map;
using std::string;
using std::vector;

class order_decider {
public:
    order_decider(map<set<string>,int>, int);
    virtual ~order_decider();
    void determine_order();
    vector<string> vars_in_order;

private:
    map<set<string>,int> original_count_map;
    unsigned int total_size;

    void determine_order(map<set<string>, int>);
    string most_occurring_with_set(set<string>,set<string>);
    int count_occurrences_with_set(string,set<string>);
    void insert_in_order(set<string>);

    set<string> vars_so_far;
};

vector<string> optimize_var_order(const spot::ltl::formula * f, int n);

} /* namespace texada */

#endif /* ORDERDECIDER_H_ */
