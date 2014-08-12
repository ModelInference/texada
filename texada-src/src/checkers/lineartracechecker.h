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

#include "../exceptions/unsupportedoperatorexception.h"

namespace texada {
using std::shared_ptr;
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

	virtual bool check(const spot::ltl::formula* node, const trace_node trace_pt);



private:
    virtual bool check(const spot::ltl::atomic_prop* node, const trace_node trace_pt);
    virtual bool check(const spot::ltl::constant* node, const trace_node trace_pt);
    virtual bool check(const spot::ltl::binop* node, const trace_node trace_pt);
    virtual bool check(const spot::ltl::unop* node,  const trace_node trace_pt);
    virtual bool check(const spot::ltl::multop* node,  const trace_node trace_pt);

    virtual trace_node get_next_event(const trace_node current_node);
    virtual bool is_terminal (const trace_node current_node);
    virtual string event_name (const trace_node current_node);

    /**
     * both the next types are not supported.
     */
    bool check(const spot::ltl::automatop* node) {
    	std::cerr << "Type automatop unsupported. \n";
    	    	return false;
    }
    bool check(const spot::ltl::bunop* node) {
    	std::cerr << "Type bunop unsupported. \n";
    	return false;
    }


};

vector<map<string, string>> valid_instants_on_traces(
        const spot::ltl::formula * prop_type,
        instants_pool_creator * instantiator,
        shared_ptr<set<vector<string_event>>> traces);

} /* namespace texada */

#endif /* LINEARTRACECHECKER2_H_ */
