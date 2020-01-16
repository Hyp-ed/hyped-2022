

# define VERBOSE to see executed commands

TARGET=testrunner
MAIN=main.test.cpp
SRCS_DIR:=src
LIBS_DIR:=lib
GTEST:=$(LIBS_DIR)/googletest
CPPCHECK:=$(LIBS_DIR)/cppcheck
GTEST_LIB_TARGET:=$(GTEST)/build/lib/libgtest.a
LINT:=../utils/Lint/presubmit.py
CPPCHECK_EXC_TARGET:=$(CPPCHECK)/cppcheck
COVERAGE_DIR:=coverage
GOOGLE_TEST_LIBS = -lgtest -Llib/googletest/build/lib/

CPPCHECK_EXEC=$(shell command -v cppcheck || echo ./$(CPPCHECK_EXC_TARGET))
CPPCHECK_ENABLE_OPS=
CPPCHECK_FLAGS=--quiet --error-exitcode=1 --inline-suppr --suppressions-list=./lib/cppcheck-suppress

VPATH=../src
# change to gnu++1 for windows using uname
include ../utils/config.mk

LFLAGS:= $(LFLAGS) ${GOOGLE_TEST_LIBS} lib/libtest.a

# test if compiler is installed
ifeq ($(shell which $(CC)), )
$(error compiler $(CC) is not installed)
endif
LL:=$(CC)


#SRCS:= $(shell find $(SRCS_DIR) -name '*.test.cpp')
include $(SRCS_DIR)/Test.files
SRCS := $(SRCS) $(MAIN)
OBJS := $(SRCS:.cpp=.o)

OBJS_DEBUG_DIR := ../$(OBJS_DEBUG_DIR)
OBJS := $(patsubst %,$(OBJS_DEBUG_DIR)/%,$(OBJS))

DEP_DIR := $(OBJS_DEBUG_DIR)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d
INC_DIR := -I$(SRCS_DIR) -I$(LIBS_DIR) -I../src/

# run "make VERBOSE=1" to see all commands
ifndef VERBOSE
	Verb := @
endif
Echo := $(Verb)echo

ifneq ($(CPPCHECK_ENABLE_OPS), )
	CPPCHECK_FLAGS+=--enable=$(CPPCHECK_ENABLE_OPS)
endif

default: runner

.PHONY: runner
runner: $(TARGET)
	$(Verb) chmod u+x testrunner
	$(Verb) cd $(ROOT); ./test/testrunner

staticcheck: $(TARGET) $(CPPCHECK_EXC_TARGET)
	$(Echo) "Starting Static Checker"
	$(Echo) "Running checker on /src ..."
	$(Verb) $(CPPCHECK_EXEC) $(CPPCHECK_FLAGS) ../src
	$(Echo) "Running checker on /test/src ..."
	$(Verb) $(CPPCHECK_EXEC) $(CPPCHECK_FLAGS) ./src
	$(Echo) "Static analysis complete"

$(TARGET): $(OBJS) lib/libtest.a
	$(Echo) "Linking executable $(MAIN) into $@"
	$(Verb) $(LL)  -o $@ $(OBJS) $(LFLAGS) $(COVERAGE_FLAGS)

$(OBJS):  $(OBJS_DEBUG_DIR)/%.o: $(SRCS_DIR)/%.cpp $(GTEST_LIB_TARGET)
	$(Echo) "Compiling $<"
	$(Verb) mkdir -p $(dir $@)
	$(Verb) $(CC) $(DEPFLAGS) $(CFLAGS) $(COVERAGE_FLAGS) -I../src -Ilib/googletest/googletest/include -o $@ -c $(INC_DIR) $<

$(GTEST_LIB_TARGET): $(GTEST).tar.gz
	$(Echo) "Building GTest Suite"
	$(Verb) chmod u+x ./lib/googletestsetup.sh
	$(Verb) ./lib/googletestsetup.sh
	$(Verb) touch $@

$(CPPCHECK_EXC_TARGET): $(CPPCHECK).tar.gz
	$(Verb) command -v cppcheck > /dev/null 2>&1 || \
	(chmod u+x ./lib/cppchecksetup.sh && ./lib/cppchecksetup.sh && touch $@)


lib/libtest.a:
	$(Verb) make -C .. test/lib/libtest.a

lint:
	$(Verb) python2.7 $(LINT) --workspace=test/src

cleanlint:
	$(Verb) rm -f .cpplint-cache

clean:
	$(Verb) rm -rf $(OBJS_DEBUG_DIR)/*
	$(Verb) rm -rf $(COVERAGE_DIR)
	$(Verb) rm -f $(TARGET)
	$(Verb) rm -f $(MAINS)
	$(Verb) rm -f lib/libtest.a

define echo_var
	@echo $(1) = $($1)
endef

info:
	$(call echo_var,CC)
	$(call echo_var,TOP)
	$(call echo_var,SRCS)
	$(call echo_var,OBJS)
#	$(call echo_var,MAINS)
	$(call echo_var,UNAME)
	$(call echo_var,CFLAGS)

-include $(OBJS:.o=.d)
