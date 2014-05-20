################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/arraytracechecker_test.cpp \
../tests/eventpermutator_test.cpp \
../tests/formulainstantiator_test.cpp 

OBJS += \
./tests/arraytracechecker_test.o \
./tests/eventpermutator_test.o \
./tests/formulainstantiator_test.o 

CPP_DEPS += \
./tests/arraytracechecker_test.d \
./tests/eventpermutator_test.d \
./tests/formulainstantiator_test.d 


# Each subdirectory must supply rules for building sources it contributes
tests/%.o: ../tests/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/spot -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


