################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
./tests/arrayinstantitator_test.cpp \
./tests/arraytracechecker_test.cpp \
./tests/formulainstantiator_test.cpp 

OBJS += \
./bin/tests/arrayinstantitator_test.o \
./bin/tests/arraytracechecker_test.o \
./bin/tests/formulainstantiator_test.o 

CPP_DEPS += \
./bin/tests/arrayinstantitator_test.d \
./bin/tests/arraytracechecker_test.d \
./bin/tests/formulainstantiator_test.d 


# Each subdirectory must supply rules for building sources it contributes
./bin/tests/%.o: ./tests/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/spot -I$(GTEST_INCL) -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


