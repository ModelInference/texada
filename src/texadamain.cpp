/*
 * texadamain.cpp
 *
 *  Created on: Jul 3, 2014
 *      Author: clemieux
 */


#include <iostream>
#include <iterator>
#include <string>
#include <boost/program_options.hpp>
#include <ltlvisit/tostring.hh>
#include "propertytypeminer.h"

int main(int ac, char* av[])
{
    try {
    	std::string formula;
    	std::string input_source;
        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("formula,f",  boost::program_options::value<std::string>()->required(), "LTL formula to mine")
        	("input_trace,t", boost::program_options::value<std::string>()->required(), "trace file to mine on");

        boost::program_options::variables_map vm;
        boost::program_options::store( boost::program_options::parse_command_line(ac, av, desc), vm);
        boost::program_options::notify(vm);


        if (vm.count("help")) {
        	std::cout << desc << "\n";
            return 0;
        }

        formula = vm["formula"].as<std::string>();
        input_source = vm["input_trace"].as<std::string>();

        std::set<const spot::ltl::formula*> found_types = texada::mine_property_type(formula,input_source);

        for (std::set<const spot::ltl::formula*>::iterator it =found_types.begin();
        		it!= found_types.end(); it++){
        	std::cout << spot::ltl::to_string(*it) << "\n";
        }


    }
    catch(std::exception& e) {
    	std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
    	std::cerr << "Exception of unknown type!\n";
    }


    return 0;
}


