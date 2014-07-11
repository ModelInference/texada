RM := rm -rf

LIBS := -lspot -lgtest -lpthread -lgtest_main -lboost_program_options

CHECKERS := texada/bin/src/checkers
INST_TOOLS := texada/bin/src/instantiation-tools
MAIN := texada/bin/src/main
PARSING := texada/bin/src/parsing
TESTS := texada/bin/tests


COMPILE_FLAGS = -std=c++11 -I$(SPOT_INCL) -I$(GTEST_INCL) -I$(BOOST_INCL) -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -pg

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
./texada/bin/src/checkers/maptracechecker.o \
./texada/bin/src/checkers/lineartracechecker.o \
./texada/bin/src/instantiation-tools/instantspoolcreator.o \
./texada/bin/src/instantiation-tools/apsubbingcloner.o \
./texada/bin/src/main/propertytypeminer.o \
./texada/bin/src/parsing/stringevent.o \
./texada/bin/src/parsing/simpleparser.o 


TEST_OBJS+= \
./texada/bin/tests/instantspoolcreator_test.o \
./texada/bin/tests/lineartracechecker_test.o \
./texada/bin/tests/apsubbingcloner_test.o \
./texada/bin/tests/maptracechecker_test.o \
./texada/bin/tests/simpleparser_test.o \
./texada/bin/tests/propertytypeminer_test.o 

# All Target
all: TexadaTest Texada

# Linking for Test
TexadaTest: $(OBJS) $(TEST_OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L$(SPOT_LIB) -L$(GTEST_LIB) -o "TexadaTest" $(OBJS) $(TEST_OBJS) $(USER_OBJS) $(LIBS) -pg
	@echo 'Finished building target: $@'
	@echo ' '

# Linking for Main Texada	
Texada: $(OBJS) ./texada/bin/src/main/texadamain.o
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L$(SPOT_LIB) -L$(GTEST_LIB) -o "Texada" ./texada/bin/src/main/texadamain.o $(OBJS) $(USER_OBJS) $(LIBS) -pg
	@echo 'Finished building target: $@'
	@echo ' '

# Compiling checkers subdir
./texada/bin/src/checkers/%.o: ./texada/src/checkers/%.cpp | $(CHECKERS)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(COMPILE_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '
	
# Compling instantiation-tools subdir
./texada/bin/src/instantiation-tools/%.o: ./texada/src/instantiation-tools/%.cpp | $(INST_TOOLS)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(COMPILE_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '
	
# Compiling main subdir
./texada/bin/src/main/%.o: ./texada/src/main/%.cpp | $(MAIN)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(COMPILE_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '
	
# Compiling parsing subdir
./texada/bin/src/parsing/%.o: ./texada/src/parsing/%.cpp | $(PARSING)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(COMPILE_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '			

# Compiling tests
./texada/bin/tests/%.o: ./texada/tests/%.cpp | $(TESTS)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(COMPILE_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '

$(CHECKERS):
	mkdir -p $@
$(INST_TOOLS):
	mkdir -p $@
$(MAIN):
	mkdir -p $@
$(PARSING):
	mkdir -p $@	
    
$(TESTS):
	mkdir -p $@	

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) Texada
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include makefile.targets
