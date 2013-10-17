
# Without this, rm -rf *.(o,so) fails
SHELL=/bin/bash

INCLUDES        := -I$(PROJECT_ROOT)/src/C++

ifdef COVERAGE
OBJDIR          := $(BUILD_ROOT)/C++/Coverage
else
OBJDIR          := $(BUILD_ROOT)/C++
endif

CXX_LIB         := $(abspath $(OBJDIR)/libConsensusCore.a)

GXX             ?= g++

ifeq ($(GXX),clang++)
    CXX_FLAGS           = $(CXX_OPT_FLAGS) -msse3 -fPIC -Qunused-arguments -fcolor-diagnostics -fno-omit-frame-pointer
    CXX_STRICT_FLAGS    = $(CXX_FLAGS)
else
    CXX_FLAGS           = $(CXX_OPT_FLAGS) $(CXX_EXTRA_ARGS) -msse3 -fPIC -fno-omit-frame-pointer
    CXX_STRICT_FLAGS    = $(CXX_FLAGS) -pedantic -ansi -Wall
endif

CXX             = $(CCACHE) $(GXX) $(MACHINE) $(CXX_FLAGS) $(INCLUDES) $(BOOST_INCLUDE)
CXX_STRICT      = $(CCACHE) $(GXX) $(MACHINE) $(CXX_STRICT_FLAGS) $(INCLUDES) $(BOOST_INCLUDE)

CXX_OPT_FLAGS_DEBUG   := -O0 -g
CXX_OPT_FLAGS_RELEASE := -O3 -DNDEBUG -g

ifeq ($(DEBUG),)
        CXX_OPT_FLAGS = $(CXX_OPT_FLAGS_RELEASE)
else
        CXX_OPT_FLAGS = $(CXX_OPT_FLAGS_DEBUG)
endif
