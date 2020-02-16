# this makefile contains common configuration that should be use throughout the project
# it defines ROOT variable which points to the root of this project

SRCS_DIR := src
OBJS_DIR := bin
TEST_DIR := test
LIBS_DIR := lib

ifeq ($(RELEASE),1)
  OBJS_DIR += /release
endif

UNAME := $(shell uname)
ifneq ($(UNAME),Linux)
  # assume Windows
  UNAME   += Windows
  CFLAGS  += -std=gnu++11
else
  CFLAGS  += -std=c++11
endif

# run "make VERBOSE=1" to see all commands
ifeq ($(VERBOSE),0)
  Verb := @
else
  Verb :=
endif
Echo := $(Verb)echo

# make recipes use space indentation instead of tabs
.RECIPEPREFIX := $(shell echo " ")

ROOT=$(shell git rev-parse --show-toplevel)
