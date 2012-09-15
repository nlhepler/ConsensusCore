#
# Tests.mk:
#  A makefile for building and running the unit tests.

# PROJECT_ROOT must be provided when this makefile is invoked.
# The working directory here is build/Tests.

# Typical invocation:
#  % cd build/Tests
#  % PROJECT_ROOT=..../ConsensusCore/ make -f .../Tests.mk <target>
# 

include $(PROJECT_ROOT)/make/Config.mk
include $(PROJECT_ROOT)/make/Defs.mk

VPATH                   := $(PROJECT_ROOT)/src/Tests
TEST_SRCS               := $(notdir $(shell find $(PROJECT_ROOT)/src/Tests -name "*.cpp" | grep -v '\#'))
TEST_OBJS               := $(TEST_SRCS:.cpp=.o)

TESTS_EXECUTABLE        := ./test-runner

run-tests: $(TESTS_EXECUTABLE)
ifdef COVERAGE
	$(LCOV)  -b $(PROJECT_ROOT) -d $(OBJDIR) --zerocounters --ignore-errors source
endif
	GTEST_OUTPUT="xml:tests-summary.xml" $(TESTS_EXECUTABLE)
ifdef COVERAGE
	$(LCOV) -b $(PROJECT_ROOT) -d $(OBJDIR) --capture -o tests.info --ignore-errors source
	$(LCOV) --extract tests.info '*/src/C++/*'  -o tests.info
	$(GENHTML) tests.info 
endif

tests: $(TESTS_EXECUTABLE)

include $(PROJECT_ROOT)/make/Extras.mk

# Test code (not the library) should always be built DEBUG
# so we can set breakpoints in it.
$(TEST_OBJS): CXX_OPT_FLAGS := $(CXX_OPT_FLAGS_DEBUG)

#
# Use libs as prerequisites here just to ensure that the tarballs get unzipped. 
#
$(TEST_OBJS) : %.o : %.cpp $(CXX_LIB) $(G_MOCK_LIBRARY) $(G_TEST_LIBRARY) $(G_CPU_PROFILER_LIBRARY)
	$(CXX) -I$(G_TEST_INCLUDE) -I$(G_MOCK_INCLUDE) -I$(G_CPU_PROFILER_INCLUDE) -c $< -o $@

$(TESTS_EXECUTABLE): $(TEST_OBJS) $(CXX_LIB) $(G_TEST_LIBRARY) $(G_MOCK_LIBRARY) $(G_CPU_PROFILER_LIBRARY) $(G_TEST_MAIN) 
	$(GXX) --coverage $^ -lpthread -o $@

.PHONY: run-tests tests
