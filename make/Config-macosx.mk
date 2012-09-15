
# Settings for Dave's Mac

DYLIB           :=so

# Default locations for things that can be overriden by environment
# variables
SWIG                ?= swig
SWIG_LIB	    ?= $(shell $(SWIG) -swiglib)
export SWIG_LIB


BOOST_INCLUDE_PATH  ?= /usr/local/boost_1_47_0
NUMPY_INCLUDE_PATH  ?= /opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages/numpy/core/include/

PYTHON_SYS_INCLUDE_PATH ?= /opt/local/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7/
PYTHON_SYS_LIBDIR       ?= /opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/config
PYTHON_SYS_LIB          ?= python2.7
