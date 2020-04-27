
# This is like build.mk but for test files
T_CFLAGS  := $(CFLAGS)
T_LFLAGS  := $(LFLAGS)
T_INC_DIR := $(INC_DIR)
T_TARGET  := $(TEST_DIR)/testrunner
T_MAIN    := main.test.cpp
T_OBJ_DIR := $(OBJS_DIR)/$(TEST_DIR)

# Defines T_SRCS
include $(TEST_DIR)/$(SRCS_DIR)/Test.files
T_SRCS      += $(T_MAIN)
T_OBJS      := $(patsubst %.cpp,$(T_OBJ_DIR)/%.o,$(T_SRCS))
T_DEPFLAGS   = -MT $@ -MMD -MP -MF $(T_OBJ_DIR)/$*.d

# Only add coverage flag if not also cross compiling
ifeq ($(RELEASE)$(CROSS), 00)
  COVERAGE_FLAGS := --coverage
endif

# libraries
GTEST_DIR     := $(TEST_DIR)/$(LIBS_DIR)/googletest
GTEST_TARGET  := $(GTEST_DIR)/build/lib/libgtest.a
T_INC_DIR     += -I$(GTEST_DIR)/googletest/include
T_LFLAGS      += -L$(dir $(GTEST_TARGET)) -lgtest
GTEST_FILTERS +=

CPPCHECK_DIR    := $(TEST_DIR)/$(LIBS_DIR)/cppcheck
CPPCHECK_TARGET := $(CPPCHECK_DIR)/cppcheck
CPPCHECK_EXEC   := $(shell command -v cppcheck || echo ./$(CPPCHECK_TARGET))
CPPCHECK_FLAGS=--quiet --error-exitcode=1 --inline-suppr --suppressions-list=$(TEST_DIR)/lib/cppcheck-suppress

# Dependency checks
## Is 1 when python is installed
PYTHON=$(shell python2.7 -V  >/dev/null 2>&1 && echo "1" )

PYTHONCHECK=$(shell [[ -z "$(PYTHON)" ]] && echo "0" || echo "1")

# Checks manual override and python flag defined in config
RUNLINTER=$(shell [[ $(NOLINT) == 0 && $(PYTHONCHECK) == 1 ]] && echo 1 )

# Pass this option to run static checker with specified severity.
# E.g. make static STATIC_ENABLE=style
STATIC_ENABLE=

.PHONY: test
test: $(T_TARGET)
	$(Verb) ./$< --gtest_filter=-*_noci

test-all: $(T_TARGET)
	$(Verb) ./$<

test-filter: $(T_TARGET)
	$(Verb) ./$< --gtest_filter=$(GTEST_FILTERS)

coverage: test
	$(Verb) ./test/utils/get_code_cov.sh

test-lint:
	$(Verb) python2.7 utils/Lint/presubmit.py --workspace=test/src

static: $(T_TARGET) $(CPPCHECK_EXEC)
	$(Echo) "Starting Static Checker"
	$(Echo) "Running checker on /src ..."
	$(Verb) $(CPPCHECK_EXEC) $(CPPCHECK_FLAGS) $(SRCS_DIR)
	$(Echo) "Running checker on /test/src ..."
	$(Verb) $(CPPCHECK_EXEC) $(CPPCHECK_FLAGS) $(TEST_DIR)/$(SRCS_DIR)
	$(Echo) "Static analysis complete"

# COMPILE TEST
$(T_TARGET): $(DEPENDENCIES) $(OBJS) $(T_OBJS)
	$(Echo) "Linking test executable $@"
	$(Verb) $(LL) -o $@ $(OBJS) $(T_OBJS) $(T_LFLAGS) $(COVERAGE_FLAGS)

$(T_OBJS): $(T_OBJ_DIR)/%.o: $(TEST_DIR)/$(SRCS_DIR)/%.cpp $(GTEST_TARGET)
	$(Echo) "Compiling $<"
	$(Verb) mkdir -p $(dir $@)
	$(Verb) $(CC) $(T_DEPFLAGS) $(T_CFLAGS) $(COVERAGE_FLAGS) -o $@ -c $(T_INC_DIR) $<

# INSTALL LIBS
$(GTEST_TARGET): $(GTEST_DIR).tar.gz
	$(Echo) "Building GTest Suite"
	$(Verb) cd test; ./lib/googletestsetup.sh
	$(Verb) touch $@

$(CPPCHECK_EXEC): $(CPPCHECK_DIR).tar.gz
	$(Verb) command -v cppcheck > /dev/null 2>&1 || \
	(chmod u+x ./test/lib/cppchecksetup.sh && ./test/lib/cppchecksetup.sh && touch $@)


test-clean:
	$(Verb) rm -rf $(T_OBJ_DIR)/
	$(Verb) rm -f $(T_TARGET)

-include $(T_OBJS:.o=.d)
