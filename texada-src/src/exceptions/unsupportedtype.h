//
// Created by Shehzad Noor Taus on 2/23/16.
//

#ifndef TEXADA_UNSUPPORTEDTYPE_H
#define TEXADA_UNSUPPORTEDTYPE_H

#include <iostream>
#include <exception>
#include "generalexception.h"

namespace texada {
    // Custom exception for unsupported type
    class unsupported_type_exception : public texada::general_exception {
    public:
        unsupported_type_exception(std::string msg);
    };
}

#endif //TEXADA_UNSUPPORTEDTYPE_H
