# This makefile configures variables related to compilation, e.g. compiler flags.
# Furthermore, it defines compilation recipes
CFLAGS          := -pthread -Wall
LFLAGS          := -lpthread -pthread
COVERAGE_FLAGS  := --coverage
CC              := g++
INC_DIR         := -I$(SRCS_DIR) -I$(LIBS_DIR)
DEPFLAGS         = -MT $@ -MMD -MP -MF $(OBJS_DIR)/$*.d

ifeq ($(RELEASE),1)
  CFLAGS += -O2
else
  CFLAGS += -Og
endif

ifneq ($(UNAME),Linux)
  # assume Windows
  UNAME   := Windows
  CFLAGS  += -DWIN -std=gnu++11
else
  CFLAGS  += -std=c++11
endif

ARCH := $(shell uname -m)
ifneq (,$(findstring 64,$(ARCH)))
  ARCH := 64
else
  ARCH := 32
endif

# Reconfigure from cross-compilation
ifeq ($(CROSS), 1)
  CC      := hyped-cross-g++
  ARCH    := 32
  LFLAGS  += -static
$(info cross-compiling)
endif

CFLAGS += -DARCH_$(ARCH)

# test if compiler is installed
ifeq ($(shell which $(CC)), )
$(warning compiler $(CC) is not installed)
endif
LL := $(CC)

# auto-discover all sources
SRCS      := $(shell find $(SRCS_DIR) -name '*.cpp')
OBJS      := $(patsubst $(SRCS_DIR)%.cpp,$(OBJS_DIR)%.o,$(SRCS))
MAIN_OBJ  := $(patsubst run/%.cpp, $(OBJS_DIR)/%.o, $(MAIN))

$(TARGET): $(OBJS) $(MAIN_OBJ)
  $(Echo) "Linking executable $(MAIN) into $@"
  $(Verb) $(LL)  -o $@ $(OBJS) $(MAIN_OBJ) $(LFLAGS)

$(MAIN_OBJ): $(OBJS_DIR)/%.o: $(MAIN)
  $(Echo) "Compiling main: $<"
  $(Verb) mkdir -p $(dir $@)
  $(Verb) $(CC) $(DEPFLAGS) $(CFLAGS) -o $@ -c $(INC_DIR) $<

$(OBJS): $(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp $(DEPENDENCIES)
  $(Echo) "Compiling $<"
  $(Verb) mkdir -p $(dir $@)
  $(Verb) $(CC) $(DEPFLAGS_RELEASE) $(CFLAGS) -o $@ -c $(INC_DIR) $<
