//
// Created by Shehzad Noor Taus on 2/23/16.
//

#include "unsupportedtype.h"

namespace texada {
    unsupported_type_exception::unsupported_type_exception(std::string message) : general_exception::general_exception(message){}
}