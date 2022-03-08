#######################
# Include local 
-include uservars.mk
#######################

ifndef TEXADA_HOME
  $(error TEXADA_HOME shell variable is not set)
endif
ifndef SPOT_LIB
  $(error SPOT_LIB variable is not set (see uservars.mk.example))
endif
ifndef GTEST_LIB
  $(error GTEST_LIB variable is not set (see uservars.mk.example))
endif
ifndef SPOT_INCL
  $(error SPOT_INCL variable is not set (see uservars.mk.example))
endif
ifndef GTEST_INCL
  $(error GTEST_INCL variable is not set (see uservars.mk.example))
endif
ifndef BOOST_INCL
  $(error BOOST_INCL variable is not set (see uservars.mk.example))
endif
ifndef BOOST_LIB
  $(error BOOST_LIB variable is not set (see uservars.mk.example))
endif

RM := rm -rf

LIBS := -lspot -lgtest -lpthread -lgtest_main -lboost_program_options -lboost_regex  -lm

TX_SRC := texada-src/
SRC := $(TX_SRC)src/
BIN_ROOT := $(TX_SRC)bin/
BIN := $(BIN_ROOT)src/
TESTS_SRC := $(TX_SRC)tests/
TESTS_BIN := $(BIN_ROOT)tests/
EXCLUDE := texadamain
MAIN := texadamain
SMTSET = 

CC := g++
CFLAGS = -std=c++17 -I$(SPOT_INCL) -I$(GTEST_INCL) -I$(BOOST_INCL) -O2 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" 

# Add inputs and outputs from these tool invocations to the build variables 

SOURCE += \
 $(wildcard $(SRC)/*/*.cpp)

OBJS += \
 $(subst texada-src/src,texada-src/bin/src,$(patsubst %.cpp,%.o,$(SOURCE))) \



FILTER_OUT = $(foreach v,$(2),$(if $(findstring $(1),$(v)),,$(v)))
OBJS_NO_MAIN := $(call FILTER_OUT,$(EXCLUDE),$(OBJS))
EXCLUDE = pptinvariantdecider
OBJS_NO_SMT := $(call FILTER_OUT,$(EXCLUDE),$(OBJS))
OBJS_NO_SMT_NO_MAIN := $(call FILTER_OUT,$(EXCLUDE),$(OBJS_NO_MAIN))

TEST_OBJS+= \
 $(subst texada-src/tests,texada-src/bin/tests, $(patsubst %.cpp,%.o,$(wildcard $(TESTS_SRC)*.cpp)))
 
TEST_OBJS_NO_SMT := $(call FILTER_OUT,$(EXCLUDE),$(TEST_OBJS))

mkdir=@mkdir -p $(@D)

# Var to check if last compilation was done with SMT flag or not

# All Target: Need to update all builds if last compilation was SMT
ifeq ($(wildcard texada),)
all: clean make-debug texada texadatest
else
all: make-debug texada texadatest
endif

# SMT Target
ifeq ($(wildcard texada-smt), )
smt: clean make-debug smtsetter texada-smt texadatest-smt
else
smt: make-debug smtsetter texada-smt texadatest-smt
endif

print-%  : ; @echo $* = $($*)

make-debug:
	@echo 'Sources: $(SOURCE)'
	@echo
	@echo 'Objs: $(OBJS)'
	@echo

smtsetter:
	$(eval SMTSET=-DSMT_SUPPORT)
	
# Linking texadatest
texadatest: $(OBJS_NO_MAIN_NO_SMT) $(TEST_OBJS_NO_SMT)
	@echo 'Linking: $@'
	$(CC) -L$(BOOST_LIB) -L$(SPOT_LIB) -L$(GTEST_LIB) -o  "texadatest" $(OBJS_NO_SMT_NO_MAIN) $(TEST_OBJS_NO_SMT) $(LIBS) 
	@echo 'Finished linking target: $@'
	@echo ' '

# Linking texada
texada: $(OBJS_NO_SMT)
	@echo 'Linking: $@'
	$(CC) -L$(BOOST_LIB) -L$(SPOT_LIB) -L$(GTEST_LIB) -o "texada" $(OBJS_NO_SMT) $(LIBS) 
	@echo 'Finished linking target: $@'
	@echo ' '
	
# Linking texada with SMT
texada-smt: $(OBJS)
	@echo 'Linking: $@'
	$(CC) -L$(SPOT_LIB) -L$(GTEST_LIB) -o "texada-smt" $(OBJS) $(LIBS) -lz3 
	@echo 'Finished linking target: $@'
	@echo ' '
	
# Linking texadatest with SMT
texadatest-smt: $(OBJS_NO_MAIN) $(TEST_OBJS)
	@echo 'Linking: $@'
	$(CC) -L$(SPOT_LIB) -L$(GTEST_LIB) -o  "texadatest-smt" $(OBJS_NO_MAIN) $(TEST_OBJS) $(LIBS) -lz3 
	@echo 'Finished linking target: $@'
	@echo ' '


# Compiling non-testing code
$(OBJS): $(BIN)%.o: $(SRC)%.cpp
	$(mkdir)
	@echo 'Building: $<'
	$(CC) $(CFLAGS) $(SMTSET)
	@echo 'Finished building: $<'
	@echo ' '

# Compiling testing code
$(TEST_OBJS): $(TESTS_BIN)%.o: $(TESTS_SRC)%.cpp
	$(mkdir)
	@echo 'Building: $<'
	$(CC) $(CFLAGS) $(SMTSET)
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
	-$(RM) $(BIN_ROOT) texadatest texada texadatest-smt texada-smt
	-@echo ' '

.PHONY: all clean make-debug smtsetter
