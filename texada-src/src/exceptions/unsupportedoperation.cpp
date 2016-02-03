#include "unsupportedoperation.h"

namespace texada {
    unsupported_operation_exception::unsupported_operation_exception(const char* message){
        msg = message;
    }

    const char* unsupported_operation_exception::what() const throw(){
        return msg;
    }
}