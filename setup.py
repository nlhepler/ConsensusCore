from distutils.command.build import build as _build
from distutils import sysconfig
from setuptools import setup

import os, re, sys
from glob import glob
from os.path import join, dirname as up

def parse_boost_version(versionFile):
    try:
        return re.search("#define BOOST_LIB_VERSION \"(.*)\"",
                         open(versionFile).read()).group(1)
    except:
        return None

def find_boost():
    """
    Look for boost in some standard filesystem places.
    """
    boosts_found = \
        glob("/usr/include/boost/version.hpp")               + \
        glob("/usr/include/boost*/boost/version.hpp")        + \
        glob("/usr/local/boost/version.hpp")                 + \
        glob("/usr/local/boost*/boost/version.hpp")          + \
        glob("/opt/local/include/boost/version.hpp")         + \
        glob("../../ThirdParty/boost/boost_1_47_0/boost/version.hpp")
    #blame mhsieh for boost_1_47_0 hack.

    best_boost_found = (None, None)

    # pick the boost with the largest version number
    for boost in boosts_found:
        version = parse_boost_version(boost)
        boost_root = up(up(boost))
        print "Boost located: %s : version %s" % (boost_root, version)
        if version > best_boost_found[1]:
            best_boost_found = (boost_root, version)
    return best_boost_found[0]


def check_swig_version(swigExecutablePath):
    """
    Verify that the swig specified is at least version 2.0.7
    Returns True if the version is OK, else False.
    """
    swig_version_pipe = os.popen(swigExecutablePath + " -version")
    swig_version_output = swig_version_pipe.read()
    swig_version_status = swig_version_pipe.close()
    if swig_version_status == None:
        swig_version_string = \
            re.search("SWIG Version (.*)\n", swig_version_output).group(1)
        print "SWIG version: %s" % (swig_version_string,)
        # lexicographic comparison
        return swig_version_string.split(".") >= ["2","0","7"]
    else:
        return False

def configure():
    """
    Look up the dependencies
     - Python.h
     - numpy headers
     - boost headers (from configuration flag)
    """
    # Python.h
    python_inc = sysconfig.get_python_inc()

    # Libpython
    python_libdir = join(sysconfig.get_python_lib(standard_lib=True), "config")
    python_lib = "python" + sysconfig.get_python_version()

    # numpy headers
    try:
        import numpy
        numpy_inc = numpy.get_include()
    except ImportError:
        print "Requires numpy >= 1.6.0"
        sys.exit(1)

    boost_inc = None
    swig_executable = "swig"

    for arg in sys.argv[:]:
        # user can supply --boost=<path>, otherwise we look
        # in some common places for it.
        if arg.find("--boost=") == 0:
            boost_inc = arg.split("=")[1]
            # validate!
            version = parse_boost_version(os.path.join(boost_inc, "boost", "version.hpp"))
            if not version:
                print "Invalid boost directory specified."
                sys.exit(1)
            sys.argv.remove(arg)

        ## user can point at swig using --swig=<path>, otherwise we just use
        ## "swig"
        elif arg.find("--swig=") == 0:
            swig_executable = arg.split("=")[1]
            sys.argv.remove(arg)

    if not boost_inc:
        # Try to find boost in a standard location.
        boost_inc = find_boost()
        if not boost_inc:
            print "Path to boost must be specified using --boost=<path>"
            sys.exit(1)

    # verify that the boost directory is sane
    if not os.path.isfile(os.path.join(boost_inc, "boost", "version.hpp")):
        print "Invalid boost location specified."
        sys.exit(1)
    print "Using boost from %s" % boost_inc

    # Verify that the swig version is adequate
    if not check_swig_version(swig_executable):
        print \
            "SWIG (version >= 2.0.7) must be in your $PATH," + \
            "or specified using --swig=<path/to/swig>.     "
        sys.exit(1)

    # handshake with the makefile is through these
    env_vars = "PYTHON_SYS_INCLUDE_PATH=%s " % python_inc      + \
               "PYTHON_SYS_LIBDIR=%s "       % python_libdir   + \
               "PYTHON_SYS_LIB=%s "          % python_lib      + \
               "NUMPY_INCLUDE_PATH=%s "      % numpy_inc       + \
               "BOOST_INCLUDE_PATH=%s "      % boost_inc       + \
               "SWIG=%s "                    % swig_executable

    return env_vars

_configuration = configure()

class build(_build):
    """
    Build the native code using the Makefile, sidestepping
    most of the horrors of Python packaging.
    """
    def run(self):
        os.system(_configuration + "make python")

    @staticmethod
    def pythonBuildDirectory():
        """
        Returns the directory where the build stashed the generated
        Python module.
        """
        return os.popen("make --no-print-directory echo-python-build-directory").read().strip()

# HACK: "setup.py install" will fail if build hasn't been called yet,
# because setuptools attempts to scan the package directory before
# invoking the build---which will fail because the directory is
# created by the build.  I consider this a bug in setuptools, and this
# is the least involved workaround I have found: inject "build" into
# the command line.
if "install" in sys.argv and not "build" in sys.argv:
    installPos = sys.argv.index("install")
    sys.argv.insert(installPos, "build")

setup(name="ConsensusCore",
      version="0.2.0",
      author="Pacific Biosciences",
      url="http://www.github.com/PacificBiosciences/ConsensusCore",
      description= \
          """A library for genomic consensus and variant calling""",
      license="BSD",
      py_modules=["ConsensusCore"],
      packages=[""],
      package_dir={"": build.pythonBuildDirectory() },

      # Smuggle the native library in as a data file
      package_data={"": ["_ConsensusCore.so"]},

      cmdclass={"build": build})
