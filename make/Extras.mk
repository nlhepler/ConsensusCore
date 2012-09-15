#
# Extras.mk:
#  A makefile for building the third-party code in extra/
#

include $(PROJECT_ROOT)/make/Config.mk
include $(PROJECT_ROOT)/make/Defs.mk

# -----------------Google mock (including Google test)  -----------------------

G_MOCK_TARBALL          :=   $(PROJECT_ROOT)/extra/gmock-1.6.0.zip
G_MOCK_ROOT             :=   $(PROJECT_ROOT)/extra/gmock-1.6.0
G_MOCK_LIBRARY          :=   $(G_MOCK_ROOT)/lib/.libs/libgmock.a
G_MOCK_INCLUDE          :=   $(G_MOCK_ROOT)/include/

G_TEST_ROOT             :=   $(G_MOCK_ROOT)/gtest
G_TEST_LIBRARY          :=   $(G_TEST_ROOT)/lib/.libs/libgtest.a
G_TEST_INCLUDE          :=   $(G_TEST_ROOT)/include/
G_TEST_MAIN             :=   $(G_TEST_ROOT)/lib/.libs/libgtest_main.a

$(G_MOCK_LIBRARY) $(G_TEST_LIBRARY) $(G_TEST_MAIN) : $(G_MOCK_TARBALL)
	cd $(PROJECT_ROOT)/extra; unzip -n $(G_MOCK_TARBALL)
	cd $(G_MOCK_ROOT) && ./configure && make
	
# --------------------- Google perftools (profilers) --------------------------

G_PERFTOOLS_TARBALL     :=   $(PROJECT_ROOT)/extra/google-perftools-1.8.3.tar.gz
G_PERFTOOLS_ROOT        :=   $(PROJECT_ROOT)/extra/google-perftools-1.8.3
G_CPU_PROFILER_LIBRARY  :=   $(G_PERFTOOLS_ROOT)/.libs/libprofiler.a
G_CPU_PROFILER_INCLUDE  :=   $(G_PERFTOOLS_ROOT)/src

$(G_CPU_PROFILER_LIBRARY): $(G_PERFTOOLS_TARBALL) 
	cd $(PROJECT_ROOT)/extra; tar xvfz $(G_PERFTOOLS_TARBALL)
	cd $(G_PERFTOOLS_ROOT) && ./configure --enable-frame-pointers && make

# ------------------------------ Cleanup --------------------------------------

clean-extras:
	-rm -rf $(G_MOCK_ROOT)
	-rm -rf $(G_PERFTOOLS_ROOT) 

.PHONY: clean-extras
