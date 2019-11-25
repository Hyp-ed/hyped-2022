CFLAGS:=-pthread -O2 -Wall
LFLAGS:=-lpthread -pthread
COVERAGE_FLAGS=--coverage
OBJS_DEBUG_DIR:=bin/debug
CC:="g++"
UNAME=$(shell uname)
ifneq ($(UNAME),Linux)
	# assume Windows
	UNAME='Windows'
	CFLAGS:=$(CFLAGS) -std=gnu++11
else
	CFLAGS:=$(CFLAGS) -std=c++11
endif

ROOT=$(shell git rev-parse --show-toplevel)
