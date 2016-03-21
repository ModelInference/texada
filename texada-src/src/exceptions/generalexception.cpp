//
// Created by Shehzad Noor Taus on 2/25/16.
//

#include "generalexception.h"


namespace texada {
    general_exception::general_exception(std::string message){
        msg = message;
    }

    const char* general_exception::what() const throw(){
        return msg.c_str();
    }
}