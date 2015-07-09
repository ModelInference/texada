/*
 * translationsparser.cpp
 *
 *  Created on: Jul 6, 2015
 *      Author: clemieux
 */

#include "translationsparser.h"

namespace texada {

translations_parser::translations_parser() {
    translations = std::make_shared<map<string,string>>();
}

translations_parser::~translations_parser() {
    // TODO Auto-generated destructor stub
}


/**
 * Parses infile in the form
 * event name 1
 * translation 1
 * event name 2
 * translation 2
 *     ...
 * event name n
 * translation n
 * To fill the map translations with
 * (event name 1) -> (translation 1)
 * (event name 2) -> (translation 2)
 *               ...
 * (event name n) -> (translation n)
 * @param infile
 */
void translations_parser::parse(std::ifstream &infile) {
    std::string line;
    // even/odd counter
    bool event_now = true;
    std::string event_name;
    while(std::getline(infile, line)){
        if (event_now){
            event_name = line;
            event_now = false;
        }
        else {
            event_now = true;
            translations->emplace(event_name,line);
        }
    }

}

/**
 * Returns the translations from this object
 * @return
 */
shared_ptr<map<string,string>> translations_parser::get_translations(){
    return translations;
}



} /* namespace texada */
