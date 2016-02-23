//
// Created by Shehzad Noor Taus on 2/23/16.
//

#ifndef TEXADA_UNSUPPORTEDTYPE_H
#define TEXADA_UNSUPPORTEDTYPE_H

#include <iostream>
#include <exception>

namespace texada {
    class unsupported_type_exception : public std::exception {
    private:
        std::string msg;
    public:
        unsupported_type_exception(std::string msg);
        virtual const char* what() const throw();
    };
}

#endif //TEXADA_UNSUPPORTEDTYPE_H
