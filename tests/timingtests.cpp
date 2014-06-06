#include "../src/propertytypeminer.h"
#include <fstream>
#include <ltlparse/public.hh>
#include "../src/simpleparser.h"
#include <ltlvisit/apcollect.hh>
#include "../src/arrayinstantiator.h"
#include "../src/formulatracechecker.h"
#include "../src/formulainstantiator.h"
#include "ltlvisit/tostring.hh"
#include <gtest/gtest.h>
#include <ltlvisit/length.hh>


/**
 * Greatly abusing the GTEST class. Will probably be subbed for some other testing method,
 * this is just preliminary timing.
 * TODO: TEMPORARY~~~~ so do this a better way (perhaps automate writing times to a file).
 *  */

double set_up_afby(std::string source_file){
	//set up the AFby formula:
	std::string input = "G(x->XFy)";
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* f = spot::ltl::parse(input, pel);
	//set up the traces
	std::ifstream infile(source_file);
	texada::simple_parser parser = texada::simple_parser();
	std::set<std::vector<texada::string_event> >  trace_set = parser.parse(infile);
	std::set<std::string> event_set = parser.return_events();
	//set up the array of instantiations
	texada::array_instantiator instantiator = texada::array_instantiator(event_set,
			*spot::ltl::atomic_prop_collect(f));
	std::vector<texada::array_instantiator::inst_fxn> instantiations;


	clock_t begin, end;
	double time_spent;

	for(std::set<std::vector<texada::string_event> >::iterator it = trace_set.begin();
			it !=trace_set.end(); it++){
		std::vector<texada::string_event> current_vec = *it;
		texada::string_event* current_trace = &current_vec[0];
		instantiator.instantiate_array();
		instantiations = instantiator.return_instantiations();
		begin = clock();
		texada::check_instants_on_trace(instantiations,f,current_trace);
		end = clock();
		time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
		}

	return time_spent / 20;

}
/*
TEST(TimingLengthTest, 250){
	std::cout << "250 \n" <<
			set_up_afby("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-250_execs-20.txt")
			<< "\n";
	ASSERT_TRUE(true);
}

TEST(TimingLengthTest, 500){
	std::cout << "500 \n " <<
			set_up_afby("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-500_execs-20.txt")
			<< "\n";
	ASSERT_TRUE(true);
}
TEST(TimingLengthTest, 750){
	std::cout << "750 \n" <<
			set_up_afby("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-750_execs-20.txt")
			<< "\n";
	ASSERT_TRUE(true);
}
TEST(TimingLengthTest, 1000){
	std::cout << "100 \n " <<
			set_up_afby("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-1000_execs-20.txt")
			<< "\n";
	ASSERT_TRUE(true);
}

TEST(TimingLengthTest, 1250){
	std::cout << "1250 \n" <<
			set_up_afby("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-1250_execs-20.txt")
			<< "\n";
	ASSERT_TRUE(true);
}

TEST(TimingLengthTest, 1500){
	std::cout << "1500 \n " <<
			set_up_afby("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-1500_execs-20.txt")
			<< "\n";
	ASSERT_TRUE(true);
}
TEST(TimingLengthTest, 1750){
	std::cout << "1750 \n " <<
			set_up_afby("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-1750_execs-20.txt")
			<< "\n";
	ASSERT_TRUE(true);
}
TEST(TimingLengthTest, 2000){
	std::cout << "2000 \n" <<
			set_up_afby("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-2000_execs-20.txt")
			<< "\n";
	ASSERT_TRUE(true);
}
TEST(TimingLengthTest, 2250){
	std::cout << "2250 \n " <<
			set_up_afby("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-2250_execs-20.txt")
			<< "\n";
	ASSERT_TRUE(true);
}*/



double set_up_form_length_2(std::string input){
	//set up the AFby formula:
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* f = spot::ltl::parse(input, pel);
	std::cout << spot::ltl::length(f) << " \n";
	//set up the traces
	std::ifstream infile("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-2250_execs-20.txt");
	texada::simple_parser parser = texada::simple_parser();
	std::set<std::vector<texada::string_event> >  trace_set = parser.parse(infile);
	std::set<std::string> event_set = parser.return_events();
	//set up the array of instantiations
	texada::array_instantiator  instantiator =  texada::array_instantiator(event_set,
			*spot::ltl::atomic_prop_collect(f));
	std::vector<texada::array_instantiator::inst_fxn> instantiations;


	clock_t begin, end;
	double time_spent;

	for(std::set<std::vector<texada::string_event> >::iterator it = trace_set.begin();
			it !=trace_set.end(); it++){
		std::vector<texada::string_event> current_vec = *it;
		texada::string_event* current_trace = &current_vec[0];
		instantiator.instantiate_array();
		instantiations = instantiator.return_instantiations();
		begin = clock();
		texada::check_instants_on_trace(instantiations,f,current_trace);
		end = clock();
		time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
		}

	return time_spent / 20;

}
/*
//Formula Length
TEST(TimingFormulaLengthTest, 3){
	std::cout << set_up_form_length_2("a | b")	<< "\n";

}TEST(TimingFormulaLengthTest, 4G){
	std::cout << set_up_form_length_2("G(a | b)")	<< "\n";

}
TEST(TimingFormulaLengthTest, 5G){
	std::cout << set_up_form_length_2("G(a->Fb)")	<< "\n";

}
TEST(TimingFormulaLengthTest, 5){
	std::cout << set_up_form_length_2("b R (a W 0)")	<< "\n";

}
TEST(TimingFormulaLengthTest, 7){
	std::cout << set_up_form_length_2("!X(!a <-> Gb)")	<< "\n";

}

TEST(TimingFormulaLengthTest, 8){
	std::cout << set_up_form_length_2("G((0 R Xb) R Gb)")	<< "\n";

}
TEST(TimingFormulaLengthTest, 9){
	std::cout << set_up_form_length_2("!(!G(a | Ga) -> b)")	<< "\n";

}
TEST(TimingFormulaLengthTest, 14){
	std::cout << set_up_form_length_2("XF(a <-> Ga) W (!XFb U Xa)")	<< "\n";

}
TEST(TimingFormulaLengthTest, 20){
	std::cout << set_up_form_length_2("(((b U (b & Fb)) R (1 U !((FXb R (b & !b)) R b))) | Ga) -> (a W b)")	<< "\n";

}
TEST(TimingFormulaLengthTest, Response){
	std::cout << set_up_form_length_2("G(a->XFb)")	<< "\n";

}
TEST(TimingFormulaLengthTest, CauseFirst){
	std::cout << set_up_form_length_2("(!bWa)&G(a->XFb)")	<< "\n";

}
TEST(TimingFormulaLengthTest, OneEffect){
	std::cout << set_up_form_length_2("(G((a->XFb )& (b->X(!b W a)))")	<< "\n";

}
TEST(TimingFormulaLengthTest, OneCause){
	std::cout << set_up_form_length_2("(G(a->X(!aUb)")	<< "\n";

}
TEST(TimingFormulaLengthTest, EffectFirst){
	std::cout << set_up_form_length_2("G(a->((X(!aUb) & XGb->X(!b W a))))")	<< "\n";

}
TEST(TimingFormulaLengthTest, MultiCause){
	std::cout << set_up_form_length_2("(!bWa)&(G(a->XFb)&(b->X(!bUa)))")	<< "\n";

}
TEST(TimingFormulaLengthTest, MultiEffect){
	std::cout << set_up_form_length_2("(!bWa)&(G(a->X(!aUb)))")	<< "\n";

}
TEST(TimingFormulaLengthTest,Alternating){
	std::cout << set_up_form_length_2("(!bWa)&(G((a->X(!aUb))&(b->X(!bUa))))")	<< "\n";

}
*/
double set_up_variable_num(std::string input, int k){
	//set up the AFby formula:
	spot::ltl::parse_error_list pel;
	const spot::ltl::formula* f = spot::ltl::parse(input, pel);
	std::cout << k << "\n";
	//set up the traces
	std::ifstream infile("/home/clemieux/workspace/texada/Texada/traces/vary-tracelen/etypes-10_events-2250_execs-20.txt");
	texada::simple_parser parser = texada::simple_parser();
	std::set<std::vector<texada::string_event> >  trace_set = parser.parse(infile);
	std::set<std::string> event_set = parser.return_events();
	//set up the array of instantiations
	texada::array_instantiator  instantiator =  texada::array_instantiator(event_set,
			*spot::ltl::atomic_prop_collect(f));
	std::vector<texada::array_instantiator::inst_fxn> instantiations;

	clock_t begin, end;
	double time_spent;

	for(std::set<std::vector<texada::string_event> >::iterator it = trace_set.begin();
			it !=trace_set.end(); it++){
		std::vector<texada::string_event> current_vec = *it;
		texada::string_event* current_trace = &current_vec[0];
		instantiator.instantiate_array();
		instantiations = instantiator.return_instantiations();
		if (k>4)
		std::cout <<"Checking instantiations on a trace... \n";
		begin = clock();
		texada::check_instants_on_trace(instantiations,f,current_trace);
		end = clock();
		time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
		}

	return time_spent / 20;

}
/*
TEST(TimingFormulaVarTest, 2){
	std::cout << set_up_variable_num("Fa -> (!b U a)",2)	<< "\n";

}
TEST(TimingFormulaVarTest, 3a){
	std::cout << set_up_variable_num("G((c & !a & Fa) -> (!b U a))",3)	<< "\n";

}
TEST(TimingFormulaVarTest, 3b){
	std::cout << set_up_variable_num("!c W (b & !a)",3)	<< "\n";

}
TEST(TimingFormulaVarTest, 3c){
	std::cout << set_up_variable_num("G((c&Fa)->((!b&!a)U(a|((b &!a)U(a|((!b&!a)U(a|((b&!a)U(a|(!bUa))))))))))",3)	<< "\n";

}
TEST(TimingFormulaVarTest, 3d){
	std::cout << set_up_variable_num("(!(b |c) W a)&G((a->X((!a U c)&(!c U b)))&(b->XFc)&(c->X((!(c|b)Wa))))",3)	<< "\n";

}
TEST(TimingFormulaVarTest, 4a){
	std::cout << set_up_variable_num("G(c & !a -> (!b W (d | a)))",4)	<< "\n";

}
TEST(TimingFormulaVarTest, 4b){
	std::cout << set_up_variable_num("(G!c) | (!c U (c & Fb -> (!b U (d & !b & X(!b U a))))",4)	<< "\n";

}

TEST(TimingFormulaVarTest,5a){
	std::cout << set_up_variable_num("G((c & Fa)->(!b U(a | (e& !b & X(!b U d)))))",5)	<< "\n";

}

TEST(TimingFormulaVarTest,5b){
	std::cout << set_up_variable_num("G(c -> (!(d & (!a) & X(!a U (e & !a))) U (a | b) | G(!(d & XFe))))",5)	<< "\n";

}
TEST(TimingFormulaVarTest,5c){
	std::cout << set_up_variable_num("G(d|c)|G(a->(Fb & G!e))",5)	<< "\n";

}*/

void set_up_total_mining_test(std::string form, std::string source){
	clock_t begin, end;
	double time_spent;
	begin = clock();
	texada::mine_property_type(form,source);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	std::cout << time_spent << "\n";

}
/*
TEST(TotalMiningNumInvsTest,4F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-3.txt");
}
TEST(TotalMiningNumInvsTest,5F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-4.txt");
}
TEST(TotalMiningNumInvsTest,6F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-5.txt");
}
TEST(TotalMiningNumInvsTest,7F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-6.txt");
}
TEST(TotalMiningNumInvsTest,8F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-7.txt");
}
TEST(TotalMiningNumInvsTest,9F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-8.txt");

}
TEST(TotalMiningNumInvsTest,13F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-12.txt");
}
TEST(TotalMiningNumInvsTest,16F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-15.txt");
}
TEST(TotalMiningNumInvsTest,19F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-18.txt");
}
TEST(TotalMiningNumInvsTest,22F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-21.txt");
}
TEST(TotalMiningNumInvsTest,25F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-24.txt");
}
TEST(TotalMiningNumInvsTest,28F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-27.txt");

}
TEST(TotalMiningNumInvsTest,31F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-30.txt");
}
TEST(TotalMiningNumInvsTest,34F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-33.txt");
}
TEST(TotalMiningNumInvsTest,37F){
	set_up_total_mining_test("Fx","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-36.txt");

}

TEST(TotalMiningNumInvsTest,4AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-3.txt");
}
TEST(TotalMiningNumInvsTest,5AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-4.txt");
}
TEST(TotalMiningNumInvsTest,6AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-5.txt");
}
TEST(TotalMiningNumInvsTest,7AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-6.txt");
}
TEST(TotalMiningNumInvsTest,8AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-7.txt");
}
TEST(TotalMiningNumInvsTest,9AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-8.txt");

}
TEST(TotalMiningNumInvsTest,13AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-12.txt");
}
TEST(TotalMiningNumInvsTest,16AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-15.txt");
}
TEST(TotalMiningNumInvsTest,19AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-18.txt");
}
TEST(TotalMiningNumInvsTest,22AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-21.txt");
}
TEST(TotalMiningNumInvsTest,25AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-24.txt");
}
TEST(TotalMiningNumInvsTest,28AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-27.txt");
}
TEST(TotalMiningNumInvsTest,31AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-30.txt");
}
TEST(TotalMiningNumInvsTest,34AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-33.txt");
}
TEST(TotalMiningNumInvsTest,37AFby){
	set_up_total_mining_test("G(x->XFy)","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-36.txt");

}

TEST(TotalMiningNumInvsTest,4PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-3.txt");
}
TEST(TotalMiningNumInvsTest,5PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-4.txt");
}
TEST(TotalMiningNumInvsTest,6PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-5.txt");
}
TEST(TotalMiningNumInvsTest,7PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-6.txt");
}
TEST(TotalMiningNumInvsTest,8PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-7.txt");
}
TEST(TotalMiningNumInvsTest,9PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-8.txt");

}
TEST(TotalMiningNumInvsTest,13PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-12.txt");
}
TEST(TotalMiningNumInvsTest,16PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-15.txt");
}
TEST(TotalMiningNumInvsTest,19PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-18.txt");
}
TEST(TotalMiningNumInvsTest,22PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-21.txt");
}
TEST(TotalMiningNumInvsTest,25PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-24.txt");
}
TEST(TotalMiningNumInvsTest,28PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-27.txt");
}
TEST(TotalMiningNumInvsTest,31PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-30.txt");
}
TEST(TotalMiningNumInvsTest,34PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-33.txt");
}
TEST(TotalMiningNumInvsTest,37PbwQR){
	set_up_total_mining_test("G((c & !a & Fa) -> (!b U a))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-36.txt");

}

TEST(TotalMiningNumInvsTest,4SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-3.txt");
}
TEST(TotalMiningNumInvsTest,5SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-4.txt");
}
TEST(TotalMiningNumInvsTest,6SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-5.txt");
}
TEST(TotalMiningNumInvsTest,7SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-6.txt");
}
TEST(TotalMiningNumInvsTest,8SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-7.txt");
}
TEST(TotalMiningNumInvsTest,9SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-8.txt");

}
TEST(TotalMiningNumInvsTest,13SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-12.txt");
}
TEST(TotalMiningNumInvsTest,16SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-15.txt");
}
TEST(TotalMiningNumInvsTest,19SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-18.txt");
}
TEST(TotalMiningNumInvsTest,22SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-21.txt");
}
TEST(TotalMiningNumInvsTest,25SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-24.txt");
}
TEST(TotalMiningNumInvsTest,28SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-27.txt");
}
TEST(TotalMiningNumInvsTest,31SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-30.txt");
}
TEST(TotalMiningNumInvsTest,34SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-33.txt");
}
TEST(TotalMiningNumInvsTest,37SpPaQuR){
	set_up_total_mining_test("G(c & !a -> (!b W (d | a)))","/home/clemieux/workspace/texada/Texada/traces/vary-invs-fixed2/log-25000_invs-36.txt");

}
*/



