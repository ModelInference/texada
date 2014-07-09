RM := rm -rf

LIBS := -lspot -lgtest -lpthread -lgtest_main -lboost_program_options

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

OBJS += \
./bin/src/arrayinstantiator.o \
./bin/src/maptracechecker.o \
./bin/src/stringevent.o \
./bin/src/lineartracechecker.o \
./bin/src/apsubbingcloner.o \
./bin/src/simpleparser.o \
./bin/src/propertytypeminer.o 


TEST_OBJS+= \
./bin/tests/arrayinstantiator_test.o \
./bin/tests/lineartracechecker_test.o \
./bin/tests/apsubbingcloner_test.o \
./bin/tests/maptracechecker_test.o \
./bin/tests/simpleparser_test.o \
./bin/tests/propertytypeminer_test.o \
./bin/tests/timingtests.o

# All Target
all: Texada texadacl

# Tool invocations
Texada: $(OBJS) $(TEST_OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L$(SPOT_LIB) -L$(GTEST_LIB) -o "Texada" $(OBJS) $(TEST_OBJS) $(USER_OBJS) $(LIBS) -pg
	@echo 'Finished building target: $@'
	@echo ' '
	
texadacl: $(OBJS) ./bin/src/texadamain.o
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L$(SPOT_LIB) -L$(GTEST_LIB) -L$(PROGOP_LIB) -o "texadacl" ./bin/src/texadamain.o $(OBJS) $(USER_OBJS) $(LIBS) -pg
	@echo 'Finished building target: $@'
	@echo ' '

# Each subdirectory must supply rules for building sources it contributes
./bin/src/%.o: ./src/%.cpp | $(BINSRC)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -I$(SPOT_INCL) -I$(GTEST_INCL) -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -pg
	@echo 'Finished building: $<'
	@echo ' '

# Each subdirectory must supply rules for building sources it contributes
./bin/tests/%.o: ./tests/%.cpp | $(BINTESTS)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -I$(SPOT_INCL) -I$(GTEST_INCL) -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -pg
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
