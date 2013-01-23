
PYTHON_LIB := $(BUILD_ROOT)/Python/_ConsensusCore.so

python-install:
	@echo "To install the ConsensusCore Python module, please use: "
	@echo "  $$ python setup.py build "
	@echo "  $$ python setup.py install "

python: $(PYTHON_LIB)

$(PYTHON_LIB): $(CXX_LIB) $(SWIG_SRCS)
	mkdir -p $(BUILD_ROOT)/Python
	$(SWIG) -builtin -python -c++ -Isrc/C++ -outdir $(BUILD_ROOT)/Python -o $(BUILD_ROOT)/Python/ConsensusCore_wrap.cxx src/SWIG/ConsensusCore.i 
	$(CXX) $(PYTHON_SYS_INCLUDES) -c $(BUILD_ROOT)/Python/ConsensusCore_wrap.cxx -o $(BUILD_ROOT)/Python/ConsensusCore_wrap.o 
	$(CXX) $(PYTHON_SHLIB_FLAGS) $(BUILD_ROOT)/Python/ConsensusCore_wrap.o $(CXX_LIB) -o $@

python-shell:
	PYTHONPATH=$(PYTHONPATH) $(PYTHON) --c="from ConsensusCore import *" -i

python-demo:
	PYTHONPATH=$(PYTHONPATH) $(PYTHON) src/Python/demo.py

clean-python:
	rm -rf $(BUILD_ROOT)/Python

echo-python-build-directory:
	@echo $(BUILD_ROOT)/Python

.PHONY: python python-shell python-demo clean-python echo-python-build-directory