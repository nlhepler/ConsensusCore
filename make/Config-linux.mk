# Linux settings

G_TOPDIR := ../..
G_PBOUT_DIR := $(G_TOPDIR)/smrtanalysis/prebuilt.out
G_BUILDOS_CMD := bash -c 'set -e; set -o pipefail; id=$$(lsb_release -si | tr "[:upper:]" "[:lower:]"); rel=$$(lsb_release -sr); case $$id in ubuntu) printf "$$id-%04d\n" $${rel/./};; centos) echo "$$id-$${rel%%.*}";; *) echo "$$id-$$rel";; esac'
G_BUILDOS := $(shell $(G_BUILDOS_CMD))

G_SWIG_DIR := $(G_PBOUT_DIR)/swig/swig-2.0.10
GA_SWIG_DIR := $(abspath $(G_SWIG_DIR))
GA_SWIG_OSDIR := $(GA_SWIG_DIR)/$(G_BUILDOS)
GA_SWIG_BINDIR := $(GA_SWIG_OSDIR)/bin
GA_SWIG_SHAREDIR := $(GA_SWIG_OSDIR)/share
GA_SWIG_SWIGLIBDIR := $(GA_SWIG_SHAREDIR)/swig/swig-2.0.10

DYLIB           :=so

# Default locations for things that can be overriden by environment
# variables.  Do not change these names---setup.py handshakes with
# make here.
SWIG                :=  $(GA_SWIG_BINDIR)/swig
SWIG_LIB            :=  $(GA_SWIG_SWIGLIBDIR)

BOOST_INCLUDE_PATH  ?=  $(PROJECT_ROOT)/../../ThirdParty/boost/boost_1_47_0/
NUMPY_INCLUDE_PATH  ?=  /usr/include/python2.7
PYTHON_SYS_INCLUDE_PATH ?= /usr/include/python2.7

PYTHON_SHLIB_FLAGS      ?= -pthread -shared -Wl,-O1
