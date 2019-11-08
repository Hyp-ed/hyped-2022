

# define VERBOSE to see executed commands

TARGET=testrunner
MAIN=main.test.cpp
SRCS_DIR:=src
OBJS_DIR:=../bin/test
LIBS_DIR:=lib
GTEST:=$(LIBS_DIR)/googletest
GTEST_LIB_DIR:=$(GTEST)/build/lib/libgtest.a
LINT:=../utils/Lint/presubmit.py

GOOGLE_TEST_LIBS = -lgtest -Llib/googletest/build/lib/

VPATH=../src
# change to gnu++1 for windows using uname 
CFLAGS:= -pthread  -O2 -Wall -Wno-unused-result 

ifneq ($(UNAME),Window)
	# assume Linux
	UNAME='Windows'
	CFLAGS:=$(CFLAGS) -std=c++11
else
	CFLAGS:=$(CFLAGS) -std=gnu++11
endif



LFLAGS:= ${GOOGLE_TEST_LIBS} lib/libtest.a

CC:="g++"

# test if compiler is installed
ifeq ($(shell which $(CC)), )
$(error compiler $(CC) is not installed)
endif
LL:=$(CC)


#SRCS:= $(shell find $(SRCS_DIR) -name '*.test.cpp')
include $(SRCS_DIR)/Test.files
SRCS := $(SRCS) $(MAIN)
OBJS := $(SRCS:.cpp=.o)

OBJS := $(patsubst %,$(OBJS_DIR)/%,$(OBJS))

DEP_DIR := $(OBJS_DIR)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d
INC_DIR := -I$(SRCS_DIR) -I$(LIBS_DIR) -I../src/

# run "make VERBOSE=1" to see all commands
ifndef VERBOSE
	Verb := @
endif
Echo := $(Verb)echo

default: runner

.PHONY: runner
runner: $(TARGET)
	chmod u+x testrunner
	./testrunner

$(TARGET): $(OBJS)
	$(Echo) "Linking executable $(MAIN) into $@"
	$(Verb) $(LL)  -o $@ $(OBJS) $(LFLAGS)

$(OBJS):  $(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(GTEST_LIB_DIR)
	$(Echo) "Compiling $<"
	$(Verb) mkdir -p $(dir $@)
	$(Verb) $(CC) $(DEPFLAGS) $(CFLAGS) -I../src -Ilib/googletest/googletest/include -o $@ -c $(INC_DIR) $<

$(GTEST_LIB_DIR): $(GTEST).tar.gz
	$(Echo) "Building GTest Suite"
	$(Verb) ./setup.sh
	$(Verb) touch $@


lint: 
	$(Verb) python2.7 $(LINT) --workspace=test/src

clean:
	$(Verb) rm -rf $(OBJS_DIR)/*
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
