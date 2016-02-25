#include "unsupportedoperation.h"

namespace texada {
    unsupported_operation_exception::unsupported_operation_exception(std::string message) : general_exception::general_exception(message){}
}