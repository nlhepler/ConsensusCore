
# Settings for building under MinGW

DYLIB                   :=dll

SWIG                    ?= $(PROJECT_ROOT)/../../ThirdParty/swigwin-2.0.4/swig.exe
SWIG_LIB	        ?= $(shell $(SWIG) -swiglib)
export SWIG_LIB

BOOST_INCLUDE_PATH      ?=  $(PROJECT_ROOT)/../../ThirdParty/boost/boost_1_47_0/
BOOST_INCLUDE           := -isystem $(BOOST)

# Need to have Mingw realign the stack when called by .NET - otherwise you will get
# crashes when using SSE.  .NET does provide the stack alignment assumed by Mingw
CXX_EXTRA_ARGS += -mstackrealign
