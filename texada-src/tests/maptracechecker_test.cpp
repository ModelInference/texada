/*
 * maptracechecker_test.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: clemieux
 */

#include "../src/checkers/maptracechecker.h"
#include "../src/parsers/mapparser.h"
#include "../src/instantiation-tools/subformulaapcollector.h"

#include <gtest/gtest.h>

#include <ltlparse/public.hh>
#include <ltlvisit/nenoform.hh>
#include <ltlvisit/tostring.hh>
#include <ltlenv/defaultenv.hh>
#include <ltlparse/public.hh>

#include <climits>
#include <fstream>
#include <stdlib.h>
#include <ctime>




// Testing map_trace_checker on a small trace, and trying to get decent code coverage
// for check and find first occ.
TEST(MapCheckerTest,SmallTrace){

    // create the trace map
    std::map<texada::event, std::vector<long>> trace_map;

    // create the position vector for a and insert into trace map
    texada::event aevent = texada::event("a");
    long aposns[] = {0,1};
    std::vector<long> apos_vec (aposns, aposns + sizeof(aposns) / sizeof(long) );
    trace_map.insert(std::pair<texada::event, std::vector<long>>(aevent,apos_vec));

    // create the position vector for b and insert into trace map
    texada::event bevent = texada::event("b");
    long bposns[] = {2,3};
    std::vector<long> bpos_vec (bposns, bposns + sizeof(bposns) / sizeof(long) );
    trace_map.insert(std::pair<texada::event, std::vector<long>>(bevent,bpos_vec));

    // create the position vector for terminal event and insert into trace map
    texada::event termvent = texada::event();
    std::vector<long> tpos_vec;
    tpos_vec.push_back(4);
    trace_map.insert(std::pair<texada::event, std::vector<long>>(termvent,tpos_vec));

    // create checker and parse error list to parse formula later on
    texada::map_trace_checker checker = texada::map_trace_checker(&trace_map);
    spot::ltl::parse_error_list pel;

    // Tests to try and cover the "check" functions

    std::string input = "G(a->Fb)";
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);
    //std::cout << "Failing?\n";
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //until
    input = "a U b";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //weak until
    input = "a W b";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //release
    input = "b R a";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //strong release
    input = "b M a";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //xor
    input = "b xor a";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //equiv, iff
    input = "b <-> a";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //impliies
    input = "a -> b";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    input = "a -> Fb";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //or
    input = "a | b";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //and
    input = "a & b";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //finally
    input = "Fa";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //globally
    input = "Ga";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    // next
    input = "Xa";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    // not
    input = "!b";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    // tests to try and cover finding first occurrence

    // first occ next
    input = "G(X!a)";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    // first occ U
    input = "G(!a R !b)";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //first occ W
    input = "G(!a M !b)";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    // first occ R
    input = "G(!b U !a)";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    // first occ M
    input = "G(!b W !a)";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //first occ G
    input = "F(G b)";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //first occ F
    input = "F(F a)";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //first occ ->
    input = "F(a->b)";
    f = spot::ltl::parse(input,pel);
    ASSERT_TRUE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //first occ xor
    input = "G(!(a xor b))";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();

    //first occ <->
    input = "F((a<->b))";
    f = spot::ltl::parse(input,pel);
    ASSERT_FALSE((checker.check_on_trace(f)).is_satisfied);
    f->destroy();
}

// testing whether map trace mines a more complex formula correctly.
TEST(MapCheckerTest,ResourceAllocation){
    if (getenv("TEXADA_HOME") == NULL){
        std::cerr << "Error: TEXADA_HOME is undefined. \n";
        FAIL();
    }
    std::string texada_home = std::string(getenv("TEXADA_HOME"));

    std::string file_source = texada_home + "/traces/resource-allocation/smallabc.txt";

    // parse the ltl formula
    spot::ltl::parse_error_list pel;
    const spot::ltl::formula* formula = spot::ltl::parse("(!(b | c) W a) & G((b -> XFc) & (a -> X((!a U c) & (!c U b))) & (c -> X(!(b | c) W a)))", pel);


    // parse log file
    std::ifstream infile(file_source);
    texada::map_parser * parser =  new texada::map_parser();
    parser->parse(infile);
    std::shared_ptr<std::set<std::map<texada::event,std::vector<long>> >>  trace_set = parser->return_map_trace();
    std::shared_ptr<std::set<std::string>>  event_set = parser->return_props();
    delete parser;

    // check
    texada::map_trace_checker checker = texada::map_trace_checker(&(*trace_set->begin()));
    ASSERT_TRUE((checker.check_on_trace(formula)).is_satisfied);
    formula->destroy();
}

// small test for memoized version.
TEST(MapCheckerTest, SmallMemoization){
  
      // create the trace map
    std::map<texada::event, std::vector<long>> trace_map;

    // create the position vector for a and insert into trace map
    texada::event aevent = texada::event("a");
    long aposns[] = {0,1};
    std::vector<long> apos_vec (aposns, aposns + sizeof(aposns) / sizeof(long) );
    trace_map.insert(std::pair<texada::event, std::vector<long>>(aevent,apos_vec));

    // create the position vector for b and insert into trace map
    texada::event bevent = texada::event("b");
    long bposns[] = {2,3};
    std::vector<long> bpos_vec (bposns, bposns + sizeof(bposns) / sizeof(long) );
    trace_map.insert(std::pair<texada::event, std::vector<long>>(bevent,bpos_vec));

    // create the position vector for terminal event and insert into trace map
    texada::event termvent = texada::event();
    std::vector<long> tpos_vec;
    tpos_vec.push_back(4);
    trace_map.insert(std::pair<texada::event, std::vector<long>>(termvent,tpos_vec));

    // create checker and parse error list to parse formula later on
    texada::map_trace_checker checker = texada::map_trace_checker(&trace_map);
    spot::ltl::parse_error_list pel;
    
    
    std::string input = "G(x->Fy)";
    const spot::ltl::formula* f = spot::ltl::parse(input, pel);
    
    // collect subformulae aps for memoization
    texada::subformula_ap_collector * collector = new texada::subformula_ap_collector();
    f->accept(*collector);
    checker.add_relevant_bindings(&collector->subform_ap_set);

    std::map<std::string,std::string> bindings_map;
    bindings_map.emplace("x","a");
    bindings_map.emplace("y","b");
    ASSERT_TRUE((checker.check_on_trace(f,bindings_map)).is_satisfied);
    f->destroy();
  
}


TEST(MapCheckerTest, DISABLED_CheckingOrder){
   clock_t t;
   if (getenv("TEXADA_HOME") == NULL){
       std::cerr << "Error: TEXADA_HOME is undefined. \n";
       FAIL();
   }

   std::ifstream infile(
           std::string(getenv("TEXADA_HOME"))
                   + "/traces/stress-tests/10-events-long-trace.txt");
   texada::map_parser * parser = new texada::map_parser;
   parser->parse(infile);
   std::shared_ptr<
           std::set<std::map<texada::event, std::vector<long>>> >trace_set =
   parser->return_map_trace();
   delete parser;
   parser = NULL;

   ASSERT_EQ(trace_set->size(), 1);
   std::cout<< "Finished parsing trace\n";

   // create checker and parse error list to parse formula later on
   texada::map_trace_checker checker = texada::map_trace_checker(&(*(trace_set->begin())));
   spot::ltl::parse_error_list pel;


   std::string input = "G((q & Fr) -> ((!p & !r) U (r | ((p & !r) U (r | ((!p & !r) U (r | ((p & !r) U (r | (!p U r)))))))))) & G((q & Fr) -> ((!s & !r) U (r | ((s & !r) U (r | ((!s & !r) U (r | ((s & !r) U (r | (!s U r)))))))))";
   const spot::ltl::formula* f = spot::ltl::parse(input, pel);
   float A[4];
   float B[4];
   float C[4];
   // collect subformulae aps for memoization
   texada::subformula_ap_collector * collector = new texada::subformula_ap_collector();
   f->accept(*collector);
   checker.add_relevant_bindings(&collector->subform_ap_set);
   std::map<std::string,std::string> bindings_map;
   int REPS = 1;

   for (int i = 0; i < 1; i++){
   std::cout << checker.num_memo_elements() << "\n";
   //A-B-C
   bindings_map.clear();
   bindings_map.emplace("p","e0");
   bindings_map.emplace("q","e1");
   bindings_map.emplace("r","e2");
   bindings_map.emplace("s","e3");
   t= clock();
   checker.check_on_trace(f,bindings_map);
   t= clock() - t;
   A[0] += ((float)t)/CLOCKS_PER_SEC;
   std::cout << "The time was " << t << " clicks, i.e. " << ((float)t)/CLOCKS_PER_SEC << " seconds.\n";
   std::cout << checker.num_memo_elements() << "\n";
   bindings_map.clear();
   bindings_map.emplace("p","e0");
   bindings_map.emplace("q","e1");
   bindings_map.emplace("r","e4");
   bindings_map.emplace("s","e5");
   t= clock();
   checker.check_on_trace(f,bindings_map);
   t= clock() - t;
   B[0] += ((float)t)/CLOCKS_PER_SEC;
   std::cout << "The time was " << t << " clicks, i.e. " << ((float)t)/CLOCKS_PER_SEC << " seconds.\n";
   std::cout << checker.num_memo_elements() << "\n";
   bindings_map.clear();
   bindings_map.emplace("p","e6");
   bindings_map.emplace("q","e7");
   bindings_map.emplace("r","e2");
   bindings_map.emplace("s","e3");
   t= clock();
   checker.check_on_trace(f,bindings_map);
   t= clock() - t;
   C[0] += ((float)t)/CLOCKS_PER_SEC;
   std::cout << "The time was " << t << " clicks, i.e. " << ((float)t)/CLOCKS_PER_SEC << " seconds.\n";
   std::cout << checker.num_memo_elements() << "\n";

   std::cout << checker.num_memo_elements() << "\n";
   checker.clear_memo();
   bindings_map.clear();
   bindings_map.emplace("p","e0");
   bindings_map.emplace("q","e1");
   bindings_map.emplace("r","e2");
   bindings_map.emplace("s","e3");
   t= clock();
   checker.check_on_trace(f,bindings_map);
   t= clock() - t;
   A[1] += ((float)t)/CLOCKS_PER_SEC;
   std::cout << "The time was " << t << " clicks, i.e. " << ((float)t)/CLOCKS_PER_SEC << " seconds.\n";
   std::cout << checker.num_memo_elements() << "\n";
   checker.clear_memo();
   bindings_map.clear();
   bindings_map.emplace("p","e0");
   bindings_map.emplace("q","e1");
   bindings_map.emplace("r","e4");
   bindings_map.emplace("s","e5");
   t= clock();
   checker.check_on_trace(f,bindings_map);
   t= clock() - t;
   B[1] += ((float)t)/CLOCKS_PER_SEC;
   std::cout << "The time was " << t << " clicks, i.e. " << ((float)t)/CLOCKS_PER_SEC << " seconds.\n";
   std::cout << checker.num_memo_elements() << "\n";
   bindings_map.clear();
   bindings_map.emplace("p","e6");
   bindings_map.emplace("q","e7");
   bindings_map.emplace("r","e2");
   bindings_map.emplace("s","e3");
   t= clock();
   checker.check_on_trace(f,bindings_map);
   t= clock() - t;
   C[1] += ((float)t)/CLOCKS_PER_SEC;
   std::cout << "The time was " << t << " clicks, i.e. " << ((float)t)/CLOCKS_PER_SEC << " seconds.\n";
   std::cout << checker.num_memo_elements() << "\n";
   checker.clear_memo();
   std::cout << checker.num_memo_elements() << "\n";
   bindings_map.clear();
   bindings_map.emplace("p","e0");
   bindings_map.emplace("q","e1");
   bindings_map.emplace("r","e4");
   bindings_map.emplace("s","e5");
   t= clock();
   checker.check_on_trace(f,bindings_map);
   t= clock() - t;
   B[2] += ((float)t)/CLOCKS_PER_SEC;
   std::cout << "The time was " << t << " clicks, i.e. " << ((float)t)/CLOCKS_PER_SEC << " seconds.\n";
   std::cout << checker.num_memo_elements() << "\n";
   bindings_map.clear();
   bindings_map.emplace("p","e6");
   bindings_map.emplace("q","e7");
   bindings_map.emplace("r","e2");
   bindings_map.emplace("s","e3");
   t= clock();
   checker.check_on_trace(f,bindings_map);
   t= clock() - t;
   C[2] += ((float)t)/CLOCKS_PER_SEC;
   std::cout << "The time was " << t << " clicks, i.e. " << ((float)t)/CLOCKS_PER_SEC << " seconds.\n";
   std::cout << checker.num_memo_elements() << "\n";
   bindings_map.clear();
   bindings_map.emplace("p","e0");
   bindings_map.emplace("q","e1");
   bindings_map.emplace("r","e2");
   bindings_map.emplace("s","e3");
   t= clock();
   checker.check_on_trace(f,bindings_map);
   t= clock() - t;
   A[2] += ((float)t)/CLOCKS_PER_SEC;
   std::cout << "The time was " << t << " clicks, i.e. " << ((float)t)/CLOCKS_PER_SEC << " seconds.\n";
   std::cout << checker.num_memo_elements() << "\n";
   checker.clear_memo();
   std::cout << checker.num_memo_elements() << "\n";
   bindings_map.clear();
   bindings_map.emplace("p","e0");
   bindings_map.emplace("q","e1");
   bindings_map.emplace("r","e4");
   bindings_map.emplace("s","e5");
   t= clock();
   checker.check_on_trace(f,bindings_map);
   t= clock() - t;
   B[3] += ((float)t)/CLOCKS_PER_SEC;
   std::cout << "The time was " << t << " clicks, i.e. " << ((float)t)/CLOCKS_PER_SEC << " seconds.\n";
   std::cout << checker.num_memo_elements() << "\n";
   bindings_map.clear();
   bindings_map.emplace("p","e6");
   bindings_map.emplace("q","e7");
   bindings_map.emplace("r","e2");
   bindings_map.emplace("s","e3");
   t= clock();
   checker.check_on_trace(f,bindings_map);
   t= clock() - t;
   C[3] += ((float)t)/CLOCKS_PER_SEC;
   std::cout << "The time was " << t << " clicks, i.e. " << ((float)t)/CLOCKS_PER_SEC << " seconds.\n";
   std::cout << checker.num_memo_elements() << "\n";
   checker.clear_memo();
   bindings_map.clear();
   bindings_map.emplace("p","e0");
   bindings_map.emplace("q","e1");
   bindings_map.emplace("r","e2");
   bindings_map.emplace("s","e3");
   t= clock();
   checker.check_on_trace(f,bindings_map);
   t= clock() - t;
   A[3] += ((float)t)/CLOCKS_PER_SEC;
   std::cout << "The time was " << t << " clicks, i.e. " << ((float)t)/CLOCKS_PER_SEC << " seconds.\n";
   std::cout << checker.num_memo_elements() << "\n";
   checker.clear_memo();
   }

   for (int i = 0; i< 4; i ++){
       std::cout << A[i]/((float)REPS) << "\n";
       std::cout << B[i]/((float)REPS) << "\n";
       std::cout << C[i]/((float)REPS) << "\n\n";
   }



   f->destroy();

}

