
# Default build configuration
# TARGET  - define the binary name to compile into
# MAIN    - define application entry point
# CROSS   - set to 1 to use a cross-compiler, to compile on a laptop and run on BBB
# NOLINT  - set to 1 to prevent linting the code
# VERBOSE - set to 1 to print all commands Makefile runs
TARGET  := hyped
MAIN    := run/main.cpp
CROSS   := 0
NOLINT  := 0
VERBOSE := 0
RELEASE := 0

# Include helper files
HELPERS_DIR  := utils/build
include $(HELPERS_DIR)/config.mk
include $(HELPERS_DIR)/build.mk
include $(HELPERS_DIR)/libs.mk
include $(HELPERS_DIR)/test.mk

.DEFAULT_GOAL := default
default: $(DEPENDENCIES) lint $(TARGET)

lint:
ifeq ($(RUNLINTER), 1)
	$(Verb) python2.7 utils/Lint/presubmit.py --workspace=src
else ifeq ($(PYTHONCHECK), 0)
	$(error Cannot find Python 2.7, please check Python Version 2.7.X  is installed)
endif

lintall:
ifeq ($(RUNLINTER), 1)
	$(Echo) -e "\nLinting src/"
	$(Verb) -python2.7 utils/Lint/presubmit.py --workspace=src
	$(Echo) -e "\nLinting run/"
	$(Verb) -python2.7 utils/Lint/presubmit.py --workspace=run
	$(Echo) -e "\nLinting test/"
	$(Verb) -python2.7 utils/Lint/presubmit.py --workspace=test
else ifeq ($(PYTHONCHECK), 0)
	$(error Cannot find Python 2.7, please check Python Version 2.7.X  is installed )
endif

clean-all: clean clean-lint test-clean

clean-lint:
	$(Verb) rm -f .cpplint-cache

clean:
	$(Verb) rm -rf $(OBJS_DIR)/
	$(Verb) rm -f $(TARGET)

define echo_var
  @echo $(1) = $($1)
endef

.PHONY: doc
doc:
	$(Verb) doxygen Doxyfile

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

USAGE := "make <target>\n"\
"useful targets:\n"\
"default  - lint check src/ code and build the main application\n"\
"           this is the default target and can be run with just \"make\"\n"\
"lint     - lint check src/ code for codestyle violations\n"\
"test     - run a unit test suite (also compile everything needed)\n"\
"clean    - delete all binary object files to recompile everything\n"\
"print VARIABLE=<variable>  - print content of a makefile variable to see that it will be when running targets\n"\
"                           - e.g. make print VARIABLE=CFLAGS\n"
.PHONY: help
help:
	$(Echo) -e $(USAGE)

-include $(OBJS:.o=.d)
