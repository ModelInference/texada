#include "unsupportedoperation.h"

namespace texada {
    unsupportedoperation::unsupportedoperation(const char* message){
        msg = message;
    }

    const char* unsupportedoperation::what() const throw(){
        return msg;
    }
}