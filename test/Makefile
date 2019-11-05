

# define VERBOSE to see executed commands

TARGET=testrunner
MAIN=main.test.cpp
SRCS_DIR:=src
OBJS_DIR:=../bin/test
LIBS_DIR:=lib
GTEST:=$(LIBS_DIR)/gtest

GOOGLE_TEST_LIBS = -lgtest -Llib/googletest/build/lib

VPATH=../src

CFLAGS:= -pthread -std=c++11 -O2 -Wall -Wno-unused-result 
LFLAGS:= ${GOOGLE_TEST_LIBS} lib/libtest.a

CC:="g++"

# test if compiler is installed
ifeq ($(shell which $(CC)), )
$(error compiler $(CC) is not installed)
endif
LL:=$(CC)


include $(SRCS_DIR)/Test.files


SRCS := $(SRCS) $(MAIN)
OBJS := $(SRCS:.cpp=.o)

SRCS := $(patsubst %,$(SRCS_DIR)/%,$(SRCS))
OBJS := $(patsubst %,$(OBJS_DIR)/%,$(OBJS))

DEP_DIR := $(OBJS_DIR)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d
INC_DIR := -I$(SRCS_DIR) -I$(LIBS_DIR) -I../src/

# run "make VERBOSE=1" to see all commands
ifndef VERBOSE
	Verb := @
endif
Echo := $(Verb)echo

default: $(TARGET)

$(TARGET): $(OBJS)
	
	$(Echo) "Linking executable $(MAIN) into $@"
	$(Verb) $(LL) $(LFLAGS) -o ../$@ $(OBJS)
	# $(Verb) $(LL)  -o $@ $(OBJS)



$(OBJS):  $(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(GTEST)
	$(Echo) "Compiling $<"
	$(Verb) mkdir -p $(dir $@)
	$(Verb) $(CC) $(DEPFLAGS) $(CFLAGS) -I../src -Ilib/googletest/googletest/include -o $@ -c $(INC_DIR) $<

$(GTEST): $(GTEST).tar.gz
	$(Echo) "Building GTest Suite"
	$(Verb) ./setup.sh
	$(Verb) touch $@


clean:
	$(Verb) rm -rf $(OBJS_DIR)/*
	$(Verb) rm -f $(TARGET)
	$(Verb) rm -f $(MAINS)
	$(Verb) rm -f lib/libtest.a
	$(Verb) rm -f lib/gtest

define echo_var
	@echo $(1) = $($1)
endef

info:
	$(call echo_var,CC)
	$(call echo_var,TOP)
#	$(call echo_var,SRCS)
	$(call echo_var,OBJS)
#	$(call echo_var,MAINS)
	$(call echo_var,UNAME)
	$(call echo_var,CFLAGS)

-include $(OBJS:.o=.d)
