
# This is like build.mk but for test files
T_CFLAGS      := $(CFLAGS)
T_LFLAGS      := $(LFLAGS)
T_INC_DIR     := $(INC_DIR)
T_TARGET      := testrunner
T_MAIN        := main.test.cpp
T_OBJ_DIR     := $(OBJS_DIR)/$(TEST_DIR)

# defines T_SRCS
include $(TEST_DIR)/$(SRCS_DIR)/Test.files
T_SRCS      += $(T_MAIN)
T_OBJS      := $(patsubst %.cpp,$(T_OBJ_DIR)/%.o,$(T_SRCS))
T_DEPFLAGS   = -MT $@ -MMD -MP -MF $(T_OBJ_DIR)/$*.d

COVERAGE_FLAGS  := --coverage

# libraries
GTEST_DIR     := $(TEST_DIR)/$(LIBS_DIR)/googletest
GTEST_TARGET  := $(GTEST_DIR)/build/lib/libgtest.a
T_INC_DIR     += -I$(GTEST_DIR)/googletest/include
T_LFLAGS      += $(GTEST_TARGET)
GTEST_FILTERS +=

CPPCHECK_DIR    := $(TEST_DIR)/$(LIBS_DIR)/cppcheck
CPPCHECK_TARGET := $(CPPCHECK_DIR)/cppcheck
CPPCHECK_EXEC   := $(shell command -v cppcheck || echo ./$(CPPCHECK_TARGET))
CPPCHECK_FLAGS=--quiet --error-exitcode=1 --inline-suppr --suppressions-list=$(TEST_DIR)/lib/cppcheck-suppress

#pass this option to run static checker with specified severity
# e.g. make static STATIC_ENABLE=style
STATIC_ENABLE=

.PHONY: test
test: $(T_TARGET)
  $(Verb) ./$< --gtest_filter=-*_noci

test-all: $(T_TARGET)
  $(Verb) ./$<

test-filter: $(T_TARGET)
  $(Verb) ./$< --gtest_filter=$(GTEST_FILTERS)

coverage: $(T_TARGET)
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
$(T_TARGET): $(OBJS) $(T_OBJS)
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

-include $(T_OBJS:.o=.d)
