
# define VERBOSE=1 to see executed commands
# default build configuration
TARGET  := hyped
MAIN    := run/main.cpp
CROSS   := 0
NOLINT  := 0
VERBOSE := 0
HELPER  := utils/build
RELEASE := 0

# TODO: move to testing makefile
#pass this option to run static checker with specified severity
# e.g. make static STATIC_ENABLE=style
STATIC_ENABLE=

# TODO: move to testing makefile
#filters for which tests to run with gtest
GTEST_FILTERS=

# include helper files
include $(HELPER)/config.mk
include $(HELPER)/build.mk

# libaries for generating TARGET
EIGEN=$(LIBS_DIR)/Eigen
RAPIDJSON=$(LIBS_DIR)/rapidjson
GITHOOKS=.git/hooks
DEPENDENCIES=$(EIGEN) $(RAPIDJSON) $(GITHOOKS)


default: lint $(TARGET)

lint:
ifeq ($(NOLINT), 0)
  $(Verb) python2.7 utils/Lint/presubmit.py --workspace=src
endif

lintall:
  $(Echo) -e "\nLinting src/"
  $(Verb) -python2.7 utils/Lint/presubmit.py --workspace=src
  $(Echo) -e "\nLinting run/"
  $(Verb) -python2.7 utils/Lint/presubmit.py --workspace=run
  $(Echo) -e "\nLinting test/"
  $(Verb) $(MAKE) -C test lint --no-print-directory

static:
  $(Verb) $(MAKE) -C test staticcheck CPPCHECK_ENABLE_OPS=$(STATIC_ENABLE)

testrunner: test/lib/libtest.a
  $(VERB) $(MAKE) -C test runner

testrunner-all: test/lib/libtest.a
  $(VERB) $(MAKE) -C test runnerall

testrunner-filter: test/lib/libtest.a
  $(VERB) $(MAKE) -C test runnerfilter GOOGLE_TEST_FILTERS=$(GTEST_FILTERS)

coverage: testrunner
  $(Verb) ./test/utils/get_code_cov.sh

test/lib/libtest.a:  $(EIGEN) $(RAPIDJSON) $(TEST_OBJS)
  $(Echo) "Making library"
  $(Verb) ar -cvr $@ $(TEST_OBJS) > /dev/null

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

.PHONY: print
print:
  $(call echo_var,$(VARIABLE))

info:
  $(call echo_var,CC)
  $(call echo_var,SRCS)
  $(call echo_var,OBJS)
  $(call echo_var,UNAME)
  $(call echo_var,CFLAGS)

# PHONY to redo even if .ccls file exists
.PHONY: .ccls
.ccls:
  $(Echo) $(CC) > $@
  $(Verb) $(foreach value,$(CFLAGS) ,echo $(value) >> $@;)
  $(Verb) $(foreach value,$(INC_DIR),echo $(value) >> $@;)

-include $(OBJS:.o=.d)
