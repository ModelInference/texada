/*
 * maptracechecker.h
 *
 *  Created on: Jun 10, 2014
 *      Author: clemieux
 */

#ifndef MAPTRACECHECKER_H_
#define MAPTRACECHECKER_H_
#include "../parsing/stringevent.h"
#include <map>
#include <unordered_map>
#include <vector>
#include <ltlast/allnodes.hh>
#include <climits>
#include <boost/functional/hash.hpp>
#include <boost/unordered_map.hpp>
#include "../instantiation-tools/instantspoolcreator.h"

namespace texada {


/**
 * Class to check whether an LTL formula holds on a trace in the form of a map.
 * Note that this class uses LONG_MAX as infinity.
 */
class map_trace_checker {

public:
    map_trace_checker(const map<string_event, vector<long>>*);
    virtual ~map_trace_checker();
    bool check_on_trace(const spot::ltl::formula *);
    shared_ptr<vector<pregen_instants_pool::inst_fxn>> check_instants_on_trace(
            shared_ptr<vector<pregen_instants_pool::inst_fxn>> instantiations,
            const spot::ltl::formula* formula);

    /**
     * This class uses relative positions to check to occurrence of events. As
     * such, it has three extra groups of functions: find first, last and all
     * occurrence(s). These take in the formula and an interval and first the
     * first, last or all occurrence(s) of the formula in that interval.
     */
private:
    struct interval {
        long start = 0;
        long end = LONG_MAX;
        bool operator==(const interval other) const {
            return (start == other.start && end == other.end);
        }
    };

    struct first_occ_storer {
        const spot::ltl::formula* formula;
        interval intvl;
        bool operator==(const first_occ_storer other) const {
            return (formula == other.formula && intvl == other.intvl);
        }
    };

    struct first_occ_storer_hash {
        std::size_t operator()(const first_occ_storer& k) const {
            using boost::hash_value;
            using boost::hash_combine;

            // Start with a hash value of formula.
            std::size_t seed = k.formula->hash();

            // Modify 'seed' by XORing and bit-shifting in
            // one member of 'Key' after the other:

            hash_combine(seed, hash_value(k.intvl.start));
            hash_combine(seed, hash_value(k.intvl.end));

            // Return the result.
            return seed;
        }
    };

    long terminal_point;
    std::unordered_map<first_occ_storer, long, first_occ_storer_hash> first_occ_map;
    std::unordered_map<first_occ_storer, long, first_occ_storer_hash> last_occ_map;
    const map<string_event, vector<long>> * trace_map;

    bool check(const spot::ltl::formula *, interval);
    bool check(const spot::ltl::atomic_prop *, interval);
    bool check(const spot::ltl::constant *, interval);
    bool check(const spot::ltl::unop *, interval);
    bool check(const spot::ltl::multop *, interval);
    bool check(const spot::ltl::binop *, interval);

    long find_first_occurrence(const spot::ltl::formula*, interval);
    long find_first_occurrence(const spot::ltl::multop*, interval);
    long find_first_occurrence(const spot::ltl::unop*, interval);
    long find_first_occurrence(const spot::ltl::constant*, interval);
    long find_first_occurrence(const spot::ltl::binop*, interval);
    long find_first_occurrence(const spot::ltl::atomic_prop*, interval);

    long return_and_add(const spot::ltl::formula*, interval, long);
    long return_and_add_end(const spot::ltl::formula*, interval, long);

    long find_last_occurrence(const spot::ltl::formula*, interval);
    long find_last_occurrence(const spot::ltl::atomic_prop*, interval);
    long find_last_occurrence(const spot::ltl::multop*, interval);
    long find_last_occurrence(const spot::ltl::unop*, interval);
    long find_last_occurrence(const spot::ltl::constant*, interval);
    long find_last_occurrence(const spot::ltl::binop*, interval);

    /**
     * automatop and bunop are not supported.
     */
    bool check(const spot::ltl::automatop* node) {
        std::cerr << "Type automatop unsupported. \n";
        return false;
    }
    bool check(const spot::ltl::bunop* node) {
        std::cerr << "Type bunop unsupported. \n";
        return false;
    }
    long find_first_occurrence(const spot::ltl::automatop*) {
        std::cerr << "Type automatop unsupported. \n";
        return -1;
    }
    long find_first_occurrence(const spot::ltl::bunop*) {
        std::cerr << "Type bunop unsupported. \n";
        return -1;
    }

    long find_last_occurrence(const spot::ltl::automatop*) {
        std::cerr << "Type automatop unsupported. \n";
        return -1;
    }
    long find_last_occurrence(const spot::ltl::bunop*) {
        std::cerr << "Type bunop unsupported. \n";
        return -1;
    }

};

} /* namespace texada */

#endif /* MAPTRACECHECKER_H_ */
