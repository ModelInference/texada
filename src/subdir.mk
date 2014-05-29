################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
./src/arrayinstantitator.cpp \
./src/arraytracechecker.cpp \
./src/formulainstantiator.cpp 

OBJS += \
./bin/src/arrayinstantitator.o \
./bin/src/arraytracechecker.o \
./bin/src/formulainstantiator.o 

CPP_DEPS += \
./bin/src/arrayinstantitator.d \
./bin/src/arraytracechecker.d \
./bin/src/formulainstantiator.d 


# Each subdirectory must supply rules for building sources it contributes
./bin/src/%.o: ./src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I$(SPOT_INCL) -I$(GTEST_INCL) -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


