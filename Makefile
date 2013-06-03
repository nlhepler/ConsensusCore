#
# Basic makefile.
#
# Notes:
#  - can compile with clang++ using:
#      % make GXX=clang++
#  - 32-bit build:
#      % make MACHINE=-m32

PROJECT_ROOT             := $(abspath .)
CONSENSUSCORE_BUILD_ROOT ?= $(PROJECT_ROOT)/build
BUILD_ROOT               := $(CONSENSUSCORE_BUILD_ROOT)

include make/Config.mk
include make/Defs.mk

VPATH           := src/C++/                     \
                  :src/C++/Matrix/              \
                  :src/C++/Quiver/              \
                  :src/C++/Quiver/detail        \
                  :src/C++/Poa/                 \
                  :src/C++/Statistics           \
                  :src/C++/Simulation           

CXX_SRCS	:= $(notdir $(shell find src/C++/ -name "*.cpp" | grep -v '\#'))
CXX_OBJS	:= $(addprefix $(OBJDIR)/, $(CXX_SRCS:.cpp=.o))
SWIG_SRCS	:= $(shell find src/SWIG/ -name "*.i")

all: $(CXX_LIB)

$(OBJDIR): 
	mkdir -p $(OBJDIR)

$(CXX_LIB): $(OBJDIR) $(CXX_OBJS) 
	ar crs $(CXX_LIB) $(CXX_OBJS)
	touch $(CXX_LIB)

$(CXX_OBJS): $(OBJDIR)/%.o: %.cpp
	$(CXX_STRICT) $(COVERAGE) -c $< -o $@
	$(CXX) -MT $(OBJDIR)/$*.o -MM $< > $(OBJDIR)/$*.d

-include $(CXX_OBJS:.o=.d)


#
# SWIG client languages
#
include make/R.mk
include make/Python.mk
include make/Java.mk
include make/CSharp.mk


#
# Convenience targets
#
demo: python-demo
shell: python-shell

docs:
	(cd doc/API; make)

lint:
	-find src -name "*.[ch]pp" | xargs ./tools/cpplint.py --verbose=0 --counting=toplevel

pre-commit-hook:
# 	for speed, apply cpplint only to changed files.
	git diff --cached --name-only --diff-filter=ACM | \
	 grep -e  '.*.[ch]pp$$' | xargs tools/cpplint.py --verbose=3

# Need rules from Extras.mk for the following.
include make/Extras.mk

clean: clean-python clean-csharp
	rm -rf $(BUILD_ROOT)/*

clean-lib:
	rm -rf $(BUILD_ROOT)/C++

clobber: clean clean-extras

tests: debug
	-(mkdir -p $(BUILD_ROOT)/Tests && \
	  cd $(BUILD_ROOT)/Tests &&       \
	  PROJECT_ROOT=$(PROJECT_ROOT)    \
	  BUILD_ROOT=$(BUILD_ROOT)        \
	  make -f $(PROJECT_ROOT)/make/Tests.mk run-tests)

test: tests
check: tests

coverage:
	make COVERAGE=--coverage
	mkdir -p $(BUILD_ROOT)/Tests/Coverage && \
	cd $(BUILD_ROOT)/Tests/Coverage &&       \
	PROJECT_ROOT=$(PROJECT_ROOT)             \
	BUILD_ROOT=$(BUILD_ROOT)                 \
	make -f $(PROJECT_ROOT)/make/Tests.mk COVERAGE=--coverage run-tests

install: python-install

check-syntax:
	$(CXX) -Wall -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)

.PHONY: all debug release demo shell docs astyle lint pre-commit-hook \
	clean clean-lib clobber tests coverage test check
