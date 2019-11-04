

# define VERBOSE to see executed commands

TARGET=hyped
MAIN=main.cpp
SRCS_DIR:=src
LIBS_DIR:=lib
OBJS_DIR:=bin

CFLAGS:=-pthread -std=c++11 -O2 -Wall -Wno-unused-result
LFLAGS:=-lpthread -pthread

# default configuration
CROSS=0
NOLINT=0
PROTOBUF=0
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


include $(SRCS_DIR)/Source.files

ifeq ($(PROTOBUF), 1)
	CFLAGS:=$(CFLAGS) $(shell pkg-config --cflags protobuf)
	LFLAGS:=$(LFLAGS) $(shell pkg-config --libs protobuf)
	SRCS:=$(SRCS) $(shell find src/telemetry -type f -name '*.cpp' | sed 's|^src/||')
endif

SRCS := $(SRCS) $(MAIN)
OBJS := $(SRCS:.cpp=.o)

SRCS := $(patsubst %,$(SRCS_DIR)/%,$(SRCS))
OBJS := $(patsubst %,$(OBJS_DIR)/%,$(OBJS))

DEP_DIR := $(OBJS_DIR)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d
INC_DIR := -I$(SRCS_DIR) -I$(LIBS_DIR)

# run "make VERBOSE=1" to see all commands
ifndef VERBOSE
	Verb := @
endif
Echo := $(Verb)echo



default: lint $(TARGET)

$(TARGET): $(EIGEN) $(OBJS)
	$(Echo) "Linking executable $(MAIN) into $@"
	$(Verb) $(LL)  -o $@ $(OBJS) $(LFLAGS)


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

libtest: $(EIGEN) $(OBJS)
	$(Echo) "Making library"
	$(Verb) ar -cvq test/$@.a $(OBJS)

clean: cleanlint cleantest
	$(Verb) rm -rf $(OBJS_DIR)/
	$(Verb) rm -f $(TARGET)
	$(Verb) rm -f $(MAINS)
	$(Verb) find src | xargs touch
	$(Verb) touch Makefile

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

protoc:
	-$(Verb) mkdir -p src/telemetry/telemetrydata
	$(Verb) protoc -I=src/telemetry --cpp_out=src/telemetry/telemetrydata message.proto
	$(Verb) mv src/telemetry/telemetrydata/message.pb.cc src/telemetry/telemetrydata/message.pb.cpp


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
