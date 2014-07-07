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
#include "tempmappropminer.h"

void set_up_total_mining_test(std::string form, std::string source){
	clock_t begin, end;
	double time_spent;
	begin = clock();
	texada::mine_property_type(form,source);
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

int main(int ac, char* av[])
{
    try {
    	std::string formula;
    	std::string input_source;
    	bool map = false;
        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("formula,f",  boost::program_options::value<std::string>(), "LTL formula to mine")
        	("input_trace,t", boost::program_options::value<std::string>(), "trace file to mine on")
        	("run_time_test", "run the formula through traces with increasing number of invariants")
        	("map_trace,m", "mine on a trace in the form of a map");

        boost::program_options::variables_map vm;
        boost::program_options::store( boost::program_options::parse_command_line(ac, av, desc), vm);
        boost::program_options::notify(vm);

        if (vm.count("map_trace")) map =true;

        if (vm.count("help")) {
        	std::cout << desc << "\n";
            return 0;
        }

        if (vm.count("formula")){
        	formula = vm["formula"].as<std::string>();
        } else{
        	std::cerr << "No inputted formula\n";
        	return 1;
        }

        if (vm.count("run_time_test")){
        	mine_on_increasing_instants(formula,map);
        	return 0;
        }
        if (vm.count("input_trace")){
        	input_source = vm["input_trace"].as<std::string>();
        } else{
            std::cerr << "No inputted trace\n";
            return 1;
        }
        std::set<const spot::ltl::formula*> found_types;
        if (map){
        	found_types =texada::mine_map_property_type(formula,input_source);
        }else{
         found_types = texada::mine_property_type(formula,input_source);}

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





