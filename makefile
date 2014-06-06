################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include makefile.init

RM := rm -rf

LIBS := -lspot -lgtest -lpthread -lgtest_main

BINSRC = bin/src
BINTESTS = bin/tests


# Include local 
-include uservars.mk

# this is auto-generated eclipse code, unsure of its necessity
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

CPP_SRCS += \
./src/arrayinstantiator.cpp \
./src/arraytracechecker.cpp \
./src/formulainstantiator.cpp \
./src/formulatracechecker.cpp \
./src/stringevent.cpp \
./src/simpleparser.cpp \
./src/propertytypeminer.cpp \
./tests/arrayinstantiator_test.cpp \
./tests/arraytracechecker_test.cpp \
./tests/formulainstantiator_test.cpp \
./tests/formulatracechecker_test.cpp \
./tests/simpleparser_test.cpp \
./tests/propertytypeminer_test.cpp \
./tests/timingtests.cpp 

OBJS += \
./bin/src/arrayinstantiator.o \
./bin/src/arraytracechecker.o \
./bin/src/formulainstantiator.o \
./bin/src/formulatracechecker.o \
./bin/src/stringevent.o \
./bin/src/simpleparser.o \
./bin/src/propertytypeminer.o \
./bin/tests/arrayinstantiator_test.o \
./bin/tests/arraytracechecker_test.o \
./bin/tests/formulainstantiator_test.o \
./bin/tests/formulatracechecker_test.o \
./bin/tests/simpleparser_test.o \
./bin/tests/propertytypeminer_test.o \
./bin/tests/timingtests.o

CPP_DEPS += \
./bin/src/arrayinstantiator.d \
./bin/src/arraytracechecker.d \
./bin/src/formulainstantiator.d \
./bin/src/formulatracechecker.d \
./bin/src/stringevent.d \
./bin/src/simpleparser.d \
./bin/src/propertytypeminer.d \
./bin/tests/arrayinstantiator_test.d \
./bin/tests/arraytracechecker_test.d \
./bin/tests/formulainstantiator_test.d \
./bin/tests/formulatracechecker_test.d \
./bin/tests/simpleparser_test.d \
./bin/tests/propertytypeminer_test.d \
./bin/tests/timingtests.d


# All Target
all: Texada

# Tool invocations
Texada: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L$(SPOT_LIB) -L$(GTEST_LIB) -o "Texada" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	
	
# Each subdirectory must supply rules for building sources it contributes
./bin/src/%.o: ./src/%.cpp | $(BINSRC)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -I$(SPOT_INCL) -I$(GTEST_INCL) -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



# Each subdirectory must supply rules for building sources it contributes
./bin/tests/%.o: ./tests/%.cpp | $(BINTESTS)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -I$(SPOT_INCL) -I$(GTEST_INCL) -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(BINSRC):
	mkdir -p bin
	mkdir -p $@
    
$(BINTESTS):
	mkdir -p bin
	mkdir -p $@	

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) Texada
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:



-include makefile.targets
