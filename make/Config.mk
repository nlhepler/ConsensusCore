# Config.mk:
#  Platform/OS specific variable definitions.  Include this
#  before including Defs.mk.

UNAME := $(shell uname)

# Make doesn't support elseif or case statements, which is
# infuriating.

ifeq ($(UNAME), Darwin)
	include $(PROJECT_ROOT)/make/Config-macosx.mk
endif

ifneq (,$(findstring MINGW, $(UNAME)))
	include $(PROJECT_ROOT)/make/Config-mingw.mk
endif

ifeq ($(UNAME), Linux)
	include $(PROJECT_ROOT)/make/Config-linux.mk
endif

BOOST_INCLUDE   := -isystem $(BOOST_INCLUDE_PATH)

DOXYGEN         := doxygen
LCOV            := lcov
GENHTML         := genhtml

PYTHON_SYS_INCLUDES := -I$(PYTHON_SYS_INCLUDE_PATH) -I$(NUMPY_INCLUDE_PATH)
PYTHON_SYS_LIBS     := -L$(PYTHON_SYS_LIBDIR) -l$(PYTHON_SYS_LIB)
PYTHON          ?= ipython --no-banner
PYTHONPATH      := src/Python:$(BUILD_ROOT)/Python:$(PYTHONPATH)
