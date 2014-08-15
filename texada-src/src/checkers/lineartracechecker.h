/*
 * lineartracechecker.h
 *
 *  Created on: May 19, 2014
 *      Author: clemieux
 */

#ifndef LINEARTRACECHECKER_H_
#define LINEARTRACECHECKER_H_

#include <ltlast/allnodes.hh>
#include <memory>
#include "../parsing/prefixtreenode.h"
#include "../parsing/stringevent.h"
#include <iostream>

#include <boost/variant.hpp>
#include <boost/unordered_map.hpp>

#include "../instantiation-tools/pregeninstantspool.h"


namespace texada {
using std::shared_ptr;
using std::set;
using boost::get;

/**
 * Class to check whether an LTL formula is true on a trace in the
 * form of a vector.
 *
 */
class linear_trace_checker {

public:
	linear_trace_checker();
	virtual ~linear_trace_checker();

    typedef boost::variant<const string_event*,shared_ptr<prefix_tree_node>> trace_node;

	virtual bool check_on_trace(const spot::ltl::formula* node, const trace_node trace_pt);



private:
	virtual map<int,bool> check(const spot::ltl::formula* node, const trace_node trace_pt, set<int> trace_ids);
	virtual map<int,bool> check_on_kids(const spot::ltl::formula* node, map<set<int>,trace_node> trace_pts, set<int> trace_ids);
    virtual map<int,bool> check(const spot::ltl::atomic_prop* node, const trace_node trace_pt, set<int> trace_ids);
    virtual map<int,bool> check(const spot::ltl::constant* node, const trace_node trace_pt, set<int> trace_ids);
    virtual map<int,bool> check(const spot::ltl::binop* node, const trace_node trace_pt, set<int> trace_ids);
    virtual map<int,bool> check(const spot::ltl::unop* node,  const trace_node trace_pt, set<int> trace_ids);
    virtual map<int,bool> check(const spot::ltl::multop* node,  const trace_node trace_pt, set<int> trace_ids);

    virtual map<set<int>,trace_node> get_next_event(const trace_node current_node);
    virtual set<int> get_trace_ids(const trace_node current_node);
    virtual bool is_terminal (const trace_node current_node);
    virtual string event_name (const trace_node current_node);

    map<int,bool> create_int_bool_map (set<int>, bool);

    map<int,bool> not_map (map<int,bool>);

    void add_satisfying_values(map<int,bool>& returned_vals, bool to_satisfy,
            map<int,bool>& map_to_return,set<int>& to_check);

    /**
     * both the next types are not supported.
     */
    map<int,bool> check(const spot::ltl::automatop* node) {
    	std::cerr << "Type automatop unsupported. \n";
    	    	return map<int,bool>();
    }
    map<int,bool> check(const spot::ltl::bunop* node) {
    	std::cerr << "Type bunop unsupported. \n";
    	return map<int,bool>();
    }


};

vector<map<string, string>> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<set<vector<string_event>>> traces);

} /* namespace texada */

#endif /* LINEARTRACECHECKER2_H_ */
