RM := rm -rf

LIBS := -lspot -lgtest -lpthread -lgtest_main -lboost_program_options -lboost_regex

CHECKERS := texada-src/bin/src/checkers
INST_TOOLS := texada-src/bin/src/instantiation-tools
MAIN := texada-src/bin/src/main
PARSING := texada-src/bin/src/parsing
TESTS := texada-src/bin/tests


COMPILE_FLAGS = -std=c++11 -I$(SPOT_INCL) -I$(GTEST_INCL) -I$(BOOST_INCL) -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"  

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
./texada-src/bin/src/checkers/maptracechecker.o \
./texada-src/bin/src/checkers/lineartracechecker.o \
./texada-src/bin/src/checkers/prefixtreechecker.o \
./texada-src/bin/src/instantiation-tools/instantspoolcreator.o \
./texada-src/bin/src/instantiation-tools/pregeninstantspool.o \
./texada-src/bin/src/instantiation-tools/otfinstantspool.o \
./texada-src/bin/src/instantiation-tools/apsubbingcloner.o \
./texada-src/bin/src/instantiation-tools/subformulaapcollector.o \
./texada-src/bin/src/main/propertytypeminer.o \
./texada-src/bin/src/parsing/stringevent.o \
./texada-src/bin/src/parsing/simpleparser.o \
./texada-src/bin/src/parsing/prefixtreenode.o \
./texada-src/bin/src/parsing/prefixtree.o


TEST_OBJS+= \
./texada-src/bin/tests/instantspoolcreator_test.o \
./texada-src/bin/tests/lineartracechecker_test.o \
./texada-src/bin/tests/apsubbingcloner_test.o \
./texada-src/bin/tests/maptracechecker_test.o \
./texada-src/bin/tests/simpleparser_test.o \
./texada-src/bin/tests/propertytypeminer_test.o \
./texada-src/bin/tests/prefixtree_test.o \
./texada-src/bin/tests/subformulaapcollector_test.o

# All Target
all: texadatest texada

# Linking for Test
texadatest: $(OBJS) $(TEST_OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L$(SPOT_LIB) -L$(GTEST_LIB) -o  "texadatest" $(OBJS) $(TEST_OBJS) $(USER_OBJS) $(LIBS) 
	@echo 'Finished building target: $@'
	@echo ' '

# Linking for Main Texada	
texada: $(OBJS) ./texada-src/bin/src/main/texadamain.o
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L$(SPOT_LIB) -L$(GTEST_LIB) -o "texada" ./texada-src/bin/src/main/texadamain.o $(OBJS) $(USER_OBJS) $(LIBS) 
	@echo 'Finished building target: $@'
	@echo ' '

# Compiling checkers subdir
./texada-src/bin/src/checkers/%.o: ./texada-src/src/checkers/%.cpp | $(CHECKERS)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(COMPILE_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '
	
# Compling instantiation-tools subdir
./texada-src/bin/src/instantiation-tools/%.o: ./texada-src/src/instantiation-tools/%.cpp | $(INST_TOOLS)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(COMPILE_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '
	
# Compiling main subdir
./texada-src/bin/src/main/%.o: ./texada-src/src/main/%.cpp | $(MAIN)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(COMPILE_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '
	
# Compiling parsing subdir
./texada-src/bin/src/parsing/%.o: ./texada-src/src/parsing/%.cpp | $(PARSING)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(COMPILE_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '			

# Compiling tests
./texada-src/bin/tests/%.o: ./texada-src/tests/%.cpp | $(TESTS)
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
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) texada texadatest
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include makefile.targets
