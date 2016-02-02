//
// Created by Shehzad Noor Taus on 2/2/16.
//

#ifndef TEXADA_UNSUPPORTEDTYPEEXCEPTION_H
#define TEXADA_UNSUPPORTEDTYPEEXCEPTION_H

#include <iostream>
#include <exception>

namespace texada {
    // Custom exception for unsupported property type
    class unsupportedoperation: public std::exception
    {
    private:
        const char* msg;
    public:
        unsupportedoperation(const char* msg);
        virtual const char* what() const throw();
    };
}

#endif //TEXADA_UNSUPPORTEDTYPEEXCEPTION_H
