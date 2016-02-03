//
// Created by Shehzad Noor Taus on 2/2/16.
//

#ifndef TEXADA_UNSUPPORTEDTYPEEXCEPTION_H
#define TEXADA_UNSUPPORTEDTYPEEXCEPTION_H

#include <iostream>
#include <exception>

namespace texada {
    // Custom exception for unsupported property type
    class unsupported_operation_exception: public std::exception
    {
    private:
        std::string msg;
    public:
        unsupported_operation_exception(std::string msg);
        virtual const char* what() const throw();
    };
}

#endif //TEXADA_UNSUPPORTEDTYPEEXCEPTION_H
