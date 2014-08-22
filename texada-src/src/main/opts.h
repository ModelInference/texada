/*
 * opts.h
 *
 *  Created on: Aug 22, 2014
 *      Author: clemieux
 */

#ifndef OPTS_H_
#define OPTS_H_

#include <boost/program_options.hpp>
#include <string>

namespace texada{

boost::program_options::variables_map set_options(std::string input_string);

boost::program_options::variables_map set_options(int ac, char* av[]);

std::vector<std::string> string_to_args(std::string commands);

boost::program_options::positional_options_description get_pos_opts_desc();

boost::program_options::options_description get_options_description();
};

#endif /* SETOPTSFORMAIN_H_ */
