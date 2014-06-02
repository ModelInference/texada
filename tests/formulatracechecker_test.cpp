/*
 * formulatracechecker_test.cpp
 *
 *  Created on: May 30, 2014
 *      Author: clemieux
 */

#include "../src/formulatracechecker.h"
#include <gtest/gtest.h>
#include <ltlparse/public.hh>
#include "../src/arrayinstantiator.h"
#include <ltlvisit/apcollect.hh>
#include "../src/stringevent.h"
#include "../src/simpleparser.h"
#include <fstream>
#include <ctime>
#include <ltlvisit/apcollect.hh>

TEST(FormulaTraceCheckerTest,AFbyOnSmallTrace){
	//set up the trace
	texada::string_event* trace = new texada::string_event[7];
	trace[0] = *(new texada::string_event("a",false));
	trace[1] = *(new texada::string_event("a",false));
	trace[2] = *(new texada::string_event("b",false));
	trace[3] = *(new texada::string_event("b",false));
	trace[4] = *(new texada::string_event("c",false));
	trace[5] = *(new texada::string_event("b",false));
	trace[6] = *(new texada::string_event("EndOfTraceVar",true));
	// set up the events
	std::set<std::string>* events = new std::set<std::string>();
	events->insert("a");
	events->insert("b");
	events->insert("c");
	//set up formula
	std::string input = "G(x->XFy)";
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* f = spot::ltl::parse(input, pel);
	//make the inst_fxn array
	texada::array_instantiator * instantiator =
			new texada::array_instantiator(events,spot::ltl::atomic_prop_collect(f));
	instantiator->instantiate_array();
	//check it all
	texada::array_instantiator::inst_fxn* checked_instants =
			texada::check_instants_on_trace(instantiator->return_instantiations(),9,f,trace);
	std::set<std::map<std::string,std::string> > * valid_instantiations =
			new std::set<std::map<std::string,std::string> >;
	for (int i=0; i<9; i++){
		if ((checked_instants+i)->validity) {
			valid_instantiations->insert((checked_instants+i)->inst_map);
		}
	}

	//creating all the valid mappings
	std::map<std::string,std::string> *paAFbyb = new std::map<std::string,std::string>();
	std::map<std::string,std::string> aAFbyb = *paAFbyb;
	aAFbyb.insert(std::pair<std::string,std::string>("x","a"));
	aAFbyb.insert(std::pair<std::string,std::string>("y","b"));
	std::map<std::string,std::string>* paAFbyc = new std::map<std::string,std::string>();
	std::map<std::string,std::string> aAFbyc = *paAFbyc;
	aAFbyc.insert(std::pair<std::string,std::string>("x","a"));
	aAFbyc.insert(std::pair<std::string,std::string>("y","c"));
	std::map<std::string,std::string>* pcAFbyb = new std::map<std::string,std::string>();
	std::map<std::string,std::string> cAFbyb = *pcAFbyb;
	cAFbyb.insert(std::pair<std::string,std::string>("x","c"));
	cAFbyb.insert(std::pair<std::string,std::string>("y","b"));

	//check that they do in fact exist in the valid instanitation map
	ASSERT_TRUE(valid_instantiations->find(aAFbyb)!=valid_instantiations->end());
	ASSERT_TRUE(valid_instantiations->find(aAFbyc)!=valid_instantiations->end());
	ASSERT_TRUE(valid_instantiations->find(cAFbyb)!=valid_instantiations->end());

}

TEST(FormulaTraceCheckerTest,AFby250){
	//set up the AFby formula:
	std::string input = "G(x->XFy)";
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* f = spot::ltl::parse(input, pel);
	//set up the traces
	std::ifstream infile("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-250_execs-20.txt");
	texada::simple_parser * parser = new texada::simple_parser();
	std::set<std::vector<texada::string_event> > * trace_set = parser->parse(infile);

	std::set<std::string> * event_set = parser->return_events();
	//set up the array of instantiations
	texada::array_instantiator * instantiator = new texada::array_instantiator(event_set,
			spot::ltl::atomic_prop_collect(f));
	instantiator->instantiate_array();
	texada::array_instantiator::inst_fxn* instantiations = instantiator->return_instantiations();


	clock_t begin, end;
	double time_spent;

	for(std::set<std::vector<texada::string_event> >::iterator it = trace_set->begin();
			it !=trace_set->end(); it++){
		int numvalid =0;
		std::vector<texada::string_event> current_vec = *it;
		texada::string_event* current_trace = &current_vec[0];
		begin = clock();
		texada::check_instants_on_trace(instantiations,100,f,current_trace);
		end = clock();
		for (int i =0; i<100; i++){
				if ((instantiations+i)->validity) numvalid++;
			}
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		std::cout << numvalid << ","<< time_spent <<"\n";
	}


	ASSERT_TRUE(true);


}



TEST(FormulaTraceCheckerTest,AFby2250){
	//set up the AFby formula:
	std::string input = "G(x->XFy)";
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* f = spot::ltl::parse(input, pel);
	//set up the traces
	std::ifstream infile("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-2250_execs-20.txt");
	texada::simple_parser * parser = new texada::simple_parser();
	std::set<std::vector<texada::string_event> > * trace_set = parser->parse(infile);

	std::set<std::string> * event_set = parser->return_events();
	//set up the array of instantiations
	texada::array_instantiator * instantiator = new texada::array_instantiator(event_set,
			spot::ltl::atomic_prop_collect(f));
	instantiator->instantiate_array();
	texada::array_instantiator::inst_fxn* instantiations = instantiator->return_instantiations();

	clock_t begin, end;
	double time_spent;

	for(std::set<std::vector<texada::string_event> >::iterator it = trace_set->begin();
			it !=trace_set->end(); it++){
		int numvalid =0;
		std::vector<texada::string_event> current_vec = *it;
		texada::string_event* current_trace = &current_vec[0];
		begin = clock();
		texada::check_instants_on_trace(instantiations,100,f,current_trace);
		end = clock();
		for (int i =0; i<100; i++){
				if ((instantiations+i)->validity) numvalid++;
			}

		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		std::cout << numvalid << ","<< time_spent <<"\n";
	}


	ASSERT_TRUE(true);
}
