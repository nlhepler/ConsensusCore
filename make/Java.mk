

java: $(CXX_OBJS)
	mkdir -p $(BUILD_ROOT)/Java/com/pacbio/analysis/ConsensusCore
	$(SWIG) -java  -package com.pacbio.analysis.ConsensusCore -c++ -Isrc/C++ -outdir $(BUILD_ROOT)/Java/com/pacbio/analysis/ConsensusCore \
	    -o $(BUILD_ROOT)/Java/ConsensusCore_wrap.cxx src/SWIG/ConsensusCore.i 
	$(CXX) $(JAVA_INCLUDES) -c $(BUILD_ROOT)/Java/ConsensusCore_wrap.cxx -o $(BUILD_ROOT)/Java/ConsensusCore_wrap.o 
	$(CXX) -shared -static-libgcc -lstdc++ -lc $(BUILD_ROOT)/Java/ConsensusCore_wrap.o $(CXX_OBJS) $(JAVA_LIBS)  -o $(BUILD_ROOT)/Java/libConsensusCore.so

build/Java/ConsensusCore.dll: $(CXX_OBJS) src/SWIG/ConsensusCore.i
	rm -Rf build/Java
	mkdir -p build/Java/com/pacbio/analysis/ConsensusCore
	$(SWIG) -java -package com.pacbio.analysis.ConsensusCore -c++ -Isrc/C++ -outdir build/Java/com/pacbio/analysis/ConsensusCore \
	    -o build/Java/ConsensusCore_wrap.cxx src/SWIG/ConsensusCore.i 
	$(CXX) $(JAVA_INCLUDES) -c build/Java/ConsensusCore_wrap.cxx -o build/Java/ConsensusCore_wrap.o 
	$(CXX) -shared -static-libgcc -static-libstdc++ -lstdc++ build/Java/ConsensusCore_wrap.o $(CXX_LIB) $(JAVA_LIBS) \
    -o build/Java/ConsensusCore.dll \
    -s -Wl,--subsystem,windows,--kill-at

java-win: build/Java/ConsensusCore.dll

java-demo:
	javac -cp $(BUILD_ROOT)/Java -d $(BUILD_ROOT)/Java src/Java/demo.java
	java -cp $(BUILD_ROOT)/Java -Djava.library.path=$(BUILD_ROOT)/Java demo

clean-java:
	rm -rf $(BUILD_ROOT)/Java

.PHONY: java java-win java-demo clean-java
