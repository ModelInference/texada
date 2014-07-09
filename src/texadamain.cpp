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

void set_up_total_mining_test(std::string form, std::string source){
	clock_t begin, end;
	double time_spent;
	begin = clock();
	texada::mine_lin_property_type(form,source);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	std::cout << time_spent << "\n";

}

void set_up_total_mining_map_test(std::string form, std::string source){
	clock_t begin, end;
	double time_spent;
	begin = clock();
	texada::mine_map_property_type(form,source);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	std::cout << time_spent << "\n";

}

void mine_on_increasing_instants(std::string form, bool map){
	std::string trace_base = "/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs";
	std::cout << form;
	if (map){
		std::cout << ", map trace: \n";
		std::cout << "4 ";
		set_up_total_mining_map_test(form,trace_base+ "-3.txt");
		std::cout << "5 ";
		set_up_total_mining_map_test(form,trace_base+ "-4.txt");
		std::cout << "6 ";
		set_up_total_mining_map_test(form,trace_base+ "-5.txt");
		std::cout << "7 ";
		set_up_total_mining_map_test(form,trace_base+ "-6.txt");
		std::cout << "8 ";
		set_up_total_mining_map_test(form,trace_base+ "-7.txt");
		std::cout << "9 ";
		set_up_total_mining_map_test(form,trace_base+ "-8.txt");
		std::cout << "13 ";
		set_up_total_mining_map_test(form,trace_base+ "-12.txt");
		std::cout << "16 ";
		set_up_total_mining_map_test(form,trace_base+ "-15.txt");
		std::cout << "19 ";
		set_up_total_mining_map_test(form,trace_base+ "-18.txt");
		std::cout << "22 ";
		set_up_total_mining_map_test(form,trace_base+ "-21.txt");
		std::cout << "25 ";
		set_up_total_mining_map_test(form,trace_base+ "-24.txt");
		std::cout << "28 ";
		set_up_total_mining_map_test(form,trace_base+ "-27.txt");
		std::cout << "31 ";
		set_up_total_mining_map_test(form,trace_base+ "-30.txt");
		std::cout << "34 ";
		set_up_total_mining_map_test(form,trace_base+ "-33.txt");
		std::cout << "37 ";
		set_up_total_mining_map_test(form,trace_base+ "-36.txt");

	}
	else{
		std::cout << ", vector trace: \n";
		std::cout << "4 ";
		set_up_total_mining_test(form,trace_base+ "-3.txt");
		std::cout << "5 ";
		set_up_total_mining_test(form,trace_base+ "-4.txt");
		std::cout << "6 ";
		set_up_total_mining_test(form,trace_base+ "-5.txt");
		std::cout << "7 ";
		set_up_total_mining_test(form,trace_base+ "-6.txt");
		std::cout << "8 ";
		set_up_total_mining_test(form,trace_base+ "-7.txt");
		std::cout << "9 ";
		set_up_total_mining_test(form,trace_base+ "-8.txt");
		std::cout << "13 ";
		set_up_total_mining_test(form,trace_base+ "-12.txt");
		std::cout << "16 ";
		set_up_total_mining_test(form,trace_base+ "-15.txt");
		std::cout << "19 ";
		set_up_total_mining_test(form,trace_base+ "-18.txt");
		std::cout << "22 ";
		set_up_total_mining_test(form,trace_base+ "-21.txt");
		std::cout << "25 ";
		set_up_total_mining_test(form,trace_base+ "-24.txt");
		std::cout << "28 ";
		set_up_total_mining_test(form,trace_base+ "-27.txt");
		std::cout << "31 ";
		set_up_total_mining_test(form,trace_base+ "-30.txt");
		std::cout << "34 ";
		set_up_total_mining_test(form,trace_base+ "-33.txt");
		std::cout << "37 ";
		set_up_total_mining_test(form,trace_base+ "-36.txt");

	}

}

/**
 * Main method
 */
int main(int ac, char* av[])
{
    try {
    	// input formula
    	std::string formula;
    	// log file source
    	std::string input_source;
    	//TODO: change map name to use_map
    	bool use_map = false;
        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("formula,f",  boost::program_options::value<std::string>(), "LTL formula to mine")
            //TODO: rename to log file
        	("log_file,l", boost::program_options::value<std::string>(), "trace file to mine on")
        	//TODO: rename to integration test
        	("run_time_test", "run the formula through traces with increasing number of invariants")
        	//TODO: vector trace mention
        	("map_trace,m", "mine on a trace in the form of a map (by default, Texada uses ");



        boost::program_options::variables_map opts_map;
        boost::program_options::store( boost::program_options::parse_command_line(ac, av, desc), opts_map);
        boost::program_options::notify(opts_map);

        //TODO: comments, logging

        if (opts_map.count("map_trace")) use_map =true;

        if (opts_map.count("help")) {
        	std::cout << desc << "\n";
            return 0;
        }

        if (opts_map.count("formula")){
        	formula = opts_map["formula"].as<std::string>();
        } else{
        	std::cerr << "No inputted formula\n";
        	return 1;
        }

        if (opts_map.count("run_time_test")){
        	mine_on_increasing_instants(formula,use_map);
        	return 0;
        }

        if (opts_map.count("log_file")){
        	input_source = opts_map["log_file"].as<std::string>();
        } else{
            std::cerr << "Did not provide log file\n";
            return 1;
        }

        // found instantiations, rename it to that
        std::set<const spot::ltl::formula*> found_types;

        // if
        if (use_map){
        	found_types = texada::mine_map_property_type(formula,input_source);
        }else {
         found_types = texada::mine_lin_property_type(formula,input_source);
        }

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





