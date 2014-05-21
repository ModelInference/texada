################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/arrayinstantitator.cpp \
../src/arraytracechecker.cpp \
../src/formulainstantiator.cpp 

OBJS += \
./src/arrayinstantitator.o \
./src/arraytracechecker.o \
./src/formulainstantiator.o 

CPP_DEPS += \
./src/arrayinstantitator.d \
./src/arraytracechecker.d \
./src/formulainstantiator.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/spot -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


