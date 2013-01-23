
# Linux settings

DYLIB           :=so

# Default locations for things that can be overriden by environment
# variables.  Do not change these names---setup.py handshakes with
# make here.
SWIG                ?=  swig
SWIG_LIB	    ?=  $(shell $(SWIG) -swiglib)
export SWIG_LIB

BOOST_INCLUDE_PATH  ?=  $(PROJECT_ROOT)/../../ThirdParty/boost/boost_1_47_0/
NUMPY_INCLUDE_PATH  ?=  /usr/include/python2.6
PYTHON_SYS_INCLUDE_PATH ?= /usr/include/python2.6

PYTHON_SHLIB_FLAGS      ?= -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions
