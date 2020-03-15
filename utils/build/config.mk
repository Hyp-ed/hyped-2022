# This makefile contains common configuration that should be use throughout the project.
# It defines ROOT variable which points to the root of this project.

SRCS_DIR := src
OBJS_DIR := bin
TEST_DIR := test
LIBS_DIR := lib

SHELL := /bin/bash

ifeq ($(RELEASE),1)
  OBJS_DIR := $(OBJS_DIR)/release
endif

UNAME := $(shell uname)

ifeq ($(UNAME), $(filter $(UNAME), Linux Darwin))
	# if Linux OR Mac
	CFLAGS := $(CFLAGS) -std=c++11
else
	# else, assume Windows
	UNAME='Windows'
	CFLAGS := $(CFLAGS) -std=gnu++11
endif

# run "make VERBOSE=1" to see all commands
ifeq ($(VERBOSE),0)
  Verb := @
else
  Verb :=
endif
Echo := $(Verb)echo

ROOT=$(shell git rev-parse --show-toplevel)
