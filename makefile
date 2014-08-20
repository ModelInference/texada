RM := rm -rf

LIBS := -lspot -lgtest -lpthread -lgtest_main -lboost_program_options

SRC := texada-src/src/
BIN_ROOT := texada-src/bin/
BIN := texada-src/bin/src/
TESTS_SRC := texada-src/tests
TESTS_BIN := texada-src/bin/tests

CHECKERS_SRC := texada-src/src/checkers
INST_TOOLS_SRC := texada-src/src/instantiation-tools
PARSING_SRC := texada-src/src/parsing

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

SOURCE_NO_MAIN += \
 $(wildcard $(CHECKERS_SRC)/*.cpp) \
 $(wildcard $(INST_TOOLS_SRC)/*.cpp) \
 $(wildcard $(PARSING_SRC)/*.cpp) \
 ./texada-src/src/main/propertytypeminer.cpp

OBJS_NO_MAIN += \
 $(subst texada-src/src,texada-src/bin/src,$(patsubst %.cpp,%.o,$(SOURCE_NO_MAIN))) \

TEST_OBJS+= \
$(subst texada-src/tests,texada-src/bin/tests, $(patsubst %.cpp,%.o,$(wildcard $(TESTS_SRC)/*.cpp)))

mkdir=@mkdir -p $(@D)

# All Target
all: make-debug texada texadatest

make-debug:
	@echo 'sources-no-main: $(SOURCE_NO_MAIN)'
	@echo
	@echo 'objs-no-main: $(OBJS_NO_MAIN)'
	@echo

# Linking for Test
texadatest: $(OBJS_NO_MAIN) $(TEST_OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L$(SPOT_LIB) -L$(GTEST_LIB) -o  "texadatest" $(OBJS_NO_MAIN) $(TEST_OBJS) $(USER_OBJS) $(LIBS) 
	@echo 'Finished building target: $@'
	@echo ' '

# Linking for Main Texada	
texada: $(OBJS_NO_MAIN) ./texada-src/bin/src/main/texadamain.o
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L$(SPOT_LIB) -L$(GTEST_LIB) -o "texada" ./texada-src/bin/src/main/texadamain.o $(OBJS_NO_MAIN) $(USER_OBJS) $(LIBS) 
	@echo 'Finished building target: $@'
	@echo ' '

# Compiling non-tests, non-main source code
$(OBJS_NO_MAIN): $(BIN)%.o: $(SRC)%.cpp
	$(mkdir)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(COMPILE_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '

$(TEST_OBJS): $(TESTS_BIN)%.o: $(TESTS_SRC)%.cpp
# Compiling tests
./texada-src/bin/tests/%.o: ./texada-src/tests/%.cpp | $(TESTS)
	$(mkdir)
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
	-$(RM) $(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) $(BIN_ROOT) texadatest
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include makefile.targets
