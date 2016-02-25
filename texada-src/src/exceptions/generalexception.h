//
// Created by Shehzad Noor Taus on 2/25/16.
//

#ifndef TEXADA_GENERALEXCEPTION_H
#define TEXADA_GENERALEXCEPTION_H


#include <iostream>
#include <exception>

namespace texada {
    class general_exception : public std::exception {
    private:
        std::string msg;
    public:
        general_exception(std::string msg);
        virtual const char* what() const throw();
    };
}

#endif //TEXADA_UNSUPPORTEDTYPE_H

