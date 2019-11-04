
# define VERBOSE to see executed commands
# default build configuration
TARGET=hyped
MAIN=run/main.cpp
CROSS=0
NOLINT=0

SRCS_DIR:=src
LIBS_DIR:=lib
OBJS_DIR:=bin

# TODO: fix use -std=gnu++11 for Windows/cygwin
CFLAGS:=-pthread -std=c++11 -O2 -Wall -Wno-unused-result
LFLAGS:=-lpthread -pthread

# libaries for generating TARGET
EIGEN=$(LIBS_DIR)/Eigen

ifeq ($(CROSS), 0)
	CC:=g++
	UNAME=$(shell uname)
	ifneq ($(UNAME),Linux)
		# assume Windows
		UNAME='Windows'
		CFLAGS:=$(CFLAGS) -DWIN
	endif
	ARCH=$(shell uname -m)
	ifneq (,$(findstring 64,$(ARCH)))
		CFLAGS:=$(CFLAGS) -DARCH_64
	endif
else
	CC:=hyped-cross-g++
	CFLAGS:=$(CFLAGS) -DARCH_32
	LFLAGS:= $(LFLAGS) -static
$(info cross-compiling)
endif

# test if compiler is installed
ifeq ($(shell which $(CC)), )
$(error compiler $(CC) is not installed)
endif
LL:=$(CC)

# auto-discover all sources
# TODO: include telemetry once protobuf dependency removed
SRCS := $(shell find $(SRCS_DIR) -name '*.cpp'  -not -path 'src/telemetry/*')
OBJS := $(patsubst $(SRCS_DIR)%.cpp,$(OBJS_DIR)%.o,$(SRCS))
MAIN_OBJ := $(patsubst run/%.cpp, $(OBJS_DIR)%.o, $(MAIN))

DEP_DIR := $(OBJS_DIR)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d
INC_DIR := -I$(SRCS_DIR) -I$(LIBS_DIR)

# run "make VERBOSE=1" to see all commands
ifndef VERBOSE
	Verb := @
endif
Echo := $(Verb)echo

default: lint $(TARGET)

$(TARGET): all-objects $(MAIN_OBJ)
	$(Echo) "Linking executable $(MAIN) into $@"
	$(Verb) $(LL)  -o $@ $(OBJS) $(MAIN_OBJ) $(LFLAGS)

$(MAIN_OBJ): $(MAIN)
ifeq (,$(wildcard $(MAIN)))
$(error file $(MAIN) does not exist)
else
	$(Echo) "Compiling $<"
	$(Verb) mkdir -p $(dir $@)
	$(Verb) $(CC) $(DEPFLAGS) $(CFLAGS) -o $@ -c $(INC_DIR) $<
endif

all-objects: $(EIGEN) | $(OBJS)

$(OBJS): $(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	$(Echo) "Compiling $<"
	$(Verb) mkdir -p $(dir $@)
	$(Verb) $(CC) $(DEPFLAGS) $(CFLAGS) -o $@ -c $(INC_DIR) $<

lint:
ifeq ($(NOLINT), 0)
	$(Verb) python2.7 utils/Lint/presubmit.py
endif

testrunner: libtest
	$(VERB) $(MAKE) -C test

libtest: all-objects
	$(Echo) "Making library"
	$(Verb) ar -cvq test/$@.a $(OBJS)

clean-all: cleanlint cleantest clean

clean:
	$(Verb) rm -rf $(OBJS_DIR)/
	$(Verb) rm -f $(TARGET)

cleanlint:
	$(Verb) rm -f .cpplint-cache

define echo_var
	@echo $(1) = $($1)
endef

cleantest:
	cd test && $(MAKE) clean
	$(Verb) rm -f testrunner

.PHONY: doc
doc:
	$(Verb) doxygen Doxyfile

# Re-install eigen library if the tar file changes
$(EIGEN): $(EIGEN).tar.gz
	$(Echo) Unpacking Eigen library $@
	$(Verb) tar -zxvf $@.tar.gz -C lib > /dev/null
	$(Verb) touch $@

info:
	$(call echo_var,CC)
	$(call echo_var,TOP)
#	$(call echo_var,SRCS)
	$(call echo_var,OBJS)
#	$(call echo_var,MAINS)
	$(call echo_var,UNAME)
	$(call echo_var,CFLAGS)

-include $(OBJS:.o=.d)
