
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

CPPCHECK_DIR    := $(LIBS_DIR)/cppcheck
CPPCHECK_TARGET := $(CPPCHECK)/cppcheck

test: $(T_TARGET)
  $(Verb) ./$< --gtest_filter=-*_noci


# static:
#   $(Verb) $(MAKE) -C test staticcheck CPPCHECK_ENABLE_OPS=$(STATIC_ENABLE)

# testrunner: test/lib/libtest.a
#   $(VERB) $(MAKE) -C test runner

# testrunner-all: test/lib/libtest.a
#   $(VERB) $(MAKE) -C test runnerall

# testrunner-filter: test/lib/libtest.a
#   $(VERB) $(MAKE) -C test runnerfilter GOOGLE_TEST_FILTERS=$(GTEST_FILTERS)

# coverage: testrunner
#   $(Verb) ./test/utils/get_code_cov.sh

# test-lint:
#   $(Verb) python2.7 $(LINT) --workspace=test/src

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

# $(CPPCHECK_TARGET)

# $(CPPCHECK_EXC_TARGET): $(CPPCHECK).tar.gz
# 	$(Verb) command -v cppcheck > /dev/null 2>&1 || \
# 	(chmod u+x ./lib/cppchecksetup.sh && ./lib/cppchecksetup.sh && touch $@)


-include $(T_OBJS:.o=.d)
