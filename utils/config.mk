CFLAGS:=-pthread -O0 -g -Wall -Wno-unused-result 
LFLAGS:=-lpthread -pthread --verbose

CC:="g++"
UNAME=$(shell uname)
ifneq ($(UNAME),Linux)
	# assume Windows
	UNAME='Windows'
	CFLAGS:=$(CFLAGS) -std=gnu++11
else
	CFLAGS:=$(CFLAGS) -std=c++11
endif
