/*
 * setoptsformain.h
 *
 *  Created on: Aug 22, 2014
 *      Author: clemieux
 */

#ifndef SETOPTSFORMAIN_H_
#define SETOPTSFORMAIN_H_

#include <boost/program_options.hpp>
#include <string>

namespace texada{

boost::program_options::variables_map set_options(bool use_string,
        std::string input_string, int ac = 0, char* av[] = NULL );

std::vector<std::string> string_to_args(std::string commands);

boost::program_options::positional_options_description get_pos_opts_desc();

boost::program_options::options_description get_options_description();
};

#endif /* SETOPTSFORMAIN_H_ */
