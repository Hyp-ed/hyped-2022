
# define VERBOSE to see executed commands
# default build configuration
TARGET=hyped
MAIN=run/main.cpp
CROSS=0
NOLINT=0

SRCS_DIR:=src
LIBS_DIR:=lib
OBJS_DIR:=bin

include utils/config.mk

# libaries for generating TARGET
EIGEN=$(LIBS_DIR)/Eigen

ifeq ($(CROSS), 0)
	ifneq ($(UNAME),Linux)
		# assume Windows
		UNAME='Windows'
		CFLAGS:=$(CFLAGS) -DWIN -std=gnu++11
	else
		CFLAGS:=$(CFLAGS) -std=c++11
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
MAIN_OBJ := $(patsubst run/%.cpp, $(OBJS_DIR)/%.o, $(MAIN))

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

$(MAIN_OBJ): $(OBJS_DIR)/%.o: $(MAIN)
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
	$(Verb) python2.7 utils/Lint/presubmit.py --workspace=src
endif

lintall:
	$(Echo) "\nLinting src/"
	$(Verb) -python2.7 utils/Lint/presubmit.py --workspace=src
	$(Echo) "\nLinting run/"
	$(Verb) -python2.7 utils/Lint/presubmit.py --workspace=run
	$(Echo) "\nLinting test/"
	$(Verb) $(MAKE) -C test lint --no-print-directory

testrunner: test/lib/libtest.a
	$(VERB) $(MAKE) -C test

test/lib/libtest.a:  $(EIGEN) $(OBJS)
	$(Echo) "Making library"
	$(Verb) ar -cvq $@ $(OBJS) > /dev/null

clean-all: cleanlint cleantest clean

clean:
	$(Verb) rm -rf $(OBJS_DIR)/
	$(Verb) rm -f $(TARGET)

cleanlint:
	$(Verb) rm -f .cpplint-cache
	$(Verb) $(MAKE) -C test cleanlint --no-print-directory

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
