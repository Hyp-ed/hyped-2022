CFLAGS:=-pthread -O2 -Wall
LFLAGS:=-lpthread -pthread
COVERAGE_FLAGS=--coverage
OBJS_DEBUG_DIR:=bin/debug
CC:="g++"
UNAME=$(shell uname)

ifeq ($(UNAME), Linux)
	CFLAGS:=$(CFLAGS) -DLINUX
endif

ifeq ($(UNAME), $(filter $(UNAME), Linux Darwin))
	# if Linux OR Mac
	CFLAGS:=$(CFLAGS) -std=c++11
else
	# else, assume Windows
	UNAME='Windows'
	CFLAGS:=$(CFLAGS) -std=gnu++11
endif

ROOT=$(shell git rev-parse --show-toplevel)
