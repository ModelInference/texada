//
// Created by Shehzad Noor Taus on 2/2/16.
//

#ifndef TEXADA_UNSUPPORTEDTYPEEXCEPTION_H
#define TEXADA_UNSUPPORTEDTYPEEXCEPTION_H

#include <iostream>
#include <exception>
#include "generalexception.h"

namespace texada {
    // Custom exception for unsupported operation
    class unsupported_operation_exception: public texada::general_exception {
    public:
        unsupported_operation_exception(std::string msg);
    };
}

#endif //TEXADA_UNSUPPORTEDTYPEEXCEPTION_H
