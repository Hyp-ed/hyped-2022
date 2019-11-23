
# define VERBOSE to see executed commands
# default build configuration
TARGET=hyped
MAIN=run/main.cpp
CROSS=0
NOLINT=0

SRCS_DIR:=src
LIBS_DIR:=lib
OBJS_DIR:=bin
OBJS_RELEASE_DIR:=${OBJS_DIR}/release


include utils/config.mk

# libaries for generating TARGET
EIGEN=$(LIBS_DIR)/Eigen
RAPIDJSON=$(LIBS_DIR)/rapidjson
GITHOOKS=.git/hooks
DEPENDENCIES=$(EIGEN) $(RAPIDJSON) $(GITHOOKS)

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
SRCS := $(shell find $(SRCS_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRCS_DIR)%.cpp,$(OBJS_RELEASE_DIR)%.o,$(SRCS))
TEST_OBJS :=  $(patsubst $(SRCS_DIR)%.cpp,$(OBJS_DEBUG_DIR)%.o,$(SRCS))
MAIN_OBJ := $(patsubst run/%.cpp, $(OBJS_RELEASE_DIR)/%.o, $(MAIN))

DEP_DIR_RELEASE := $(OBJS_RELEASE_DIR)
DEPFLAGS_RELEASE = -MT $@ -MMD -MP -MF $(DEP_DIR_RELEASE)/$*.d

DEP_DIR_DEBUG := $(OBJS_DEBUG_DIR)
DEPFLAGS_DEBUG = -MT $@ -MMD -MP -MF $(DEP_DIR_DEBUG)/$*.d

INC_DIR := -I$(SRCS_DIR) -I$(LIBS_DIR)

# run "make VERBOSE=1" to see all commands
ifndef VERBOSE
	Verb := @
endif
Echo := $(Verb)echo

default: lint $(TARGET)

$(TARGET): $(OBJS) $(MAIN_OBJ)
	$(Echo) "Linking executable $(MAIN) into $@"
	$(Verb) $(LL)  -o $@ $(OBJS) $(MAIN_OBJ) $(LFLAGS)

$(MAIN_OBJ): $(OBJS_RELEASE_DIR)/%.o: $(MAIN)
ifeq (,$(wildcard $(MAIN)))
$(error file $(MAIN) does not exist)
else
	$(Echo) "Compiling $<"
	$(Verb) mkdir -p $(dir $@)
	$(Verb) $(CC) $(DEPFLAGS_RELEASE) $(CFLAGS) -o $@ -c $(INC_DIR) $<
endif

$(OBJS): $(OBJS_RELEASE_DIR)/%.o: $(SRCS_DIR)/%.cpp $(DEPENDENCIES)
	$(Echo) "Compiling $<"
	$(Verb) mkdir -p $(dir $@)
	$(Verb) $(CC) $(DEPFLAGS_RELEASE) $(CFLAGS) -o $@ -c $(INC_DIR) $<


$(TEST_OBJS): $(OBJS_DEBUG_DIR)/%.o: $(SRCS_DIR)/%.cpp $(DEPENDENCIES)
		$(Echo) "Compiling $<"
		$(Verb) mkdir -p $(dir $@)
		$(Verb) $(CC) $(DEPFLAGS_DEBUG) $(CFLAGS) -o $@ -c $(INC_DIR) $< ${COVERAGE_FLAGS}
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
	$(VERB) $(MAKE) -C test runner

coverage: testrunner
	$(Verb) ./test/utils/get_code_cov.sh

test/lib/libtest.a:  $(EIGEN) $(RAPIDJSON) $(TEST_OBJS)
	$(Echo) "Making library"
	$(Verb) ar -cvq $@ $(TEST_OBJS) > /dev/null

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

# Re-install rapidjson library if the tar file changes
$(RAPIDJSON): $(RAPIDJSON).tar.gz
	$(Echo) Unpacking RapidJSON library $@
	$(Verb) tar -zxvf $@.tar.gz -C lib > /dev/null
	$(Verb) touch $@

$(GITHOOKS): utils/githooks/*
	$(Echo) New githooks, installing
	$(Verb) ./setup.sh

info:
	$(call echo_var,CC)
	$(call echo_var,TOP)
#	$(call echo_var,SRCS)
	$(call echo_var,OBJS)
	$(call echo_var,TEST_OBJS)
#	$(call echo_var,MAINS)
	$(call echo_var,UNAME)
	$(call echo_var,CFLAGS)

-include $(OBJS:.o=.d)
