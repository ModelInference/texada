/*
 * translationsparser.h
 *
 *  Created on: Jul 6, 2015
 *      Author: clemieux
 */

#ifndef TRANSLATIONSPARSER_H_
#define TRANSLATIONSPARSER_H_

#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <fstream>

namespace texada {

using std::string;
using std::map;
using std::shared_ptr;

/**
 * Parses event name -> event translation in smtlibv2 format files,
 * ASSUMING the format is
 * event name #1
 * translation #1
 * event name #2
 * translation #2
 * ...
 * event name #n
 * translation #n
 */
class translations_parser {
public:
    translations_parser();
    virtual ~translations_parser();
    void parse(std::ifstream&);
    shared_ptr<map<string,string>> get_translations();
private:
    shared_ptr<map<string,string>>  translations;
};

} /* namespace texada */

#endif /* TRANSLATIONSPARSER_H_ */
