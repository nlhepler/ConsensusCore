
CSHARP_COMPILE           := gmcs -nowarn:0114
CSHARP_LINK_NATIVE_FLAGS := -shared -static-libgcc -lstdc++ 

CSHARP_CIL_LIB           := $(BUILD_ROOT)/CSharp/ConsensusCore.dll
CSHARP_NATIVE_LIB        := $(BUILD_ROOT)/CSharp/libConsensusCore_.$(DYLIB)

# Location expected by our .NET ecosystem.
CSHARP_CIL_LIB_PACBIO    := $(PROJECT_ROOT)/csproj/bin/Debug/ConsensusCore.dll 
CSHARP_NATIVE_LIB_PACBIO := $(PROJECT_ROOT)/csproj/bin/Debug/libConsensusCore_.$(DYLIB) 

csharp: $(CSHARP_CIL_LIB) $(CSHARP_NATIVE_LIB)

csharp-pacbio: SWIG := ../../prebuilt.out/tools/swig-2.0.4/bin/swig
csharp-pacbio: $(CSHARP_NATIVE_LIB_PACBIO)

# Customized build rules for getting a p/invoke-able dll from mingw on windows.
# See: http://www.transmissionzero.co.uk/computing/advanced-mingw-dll-topics/ for background
# You need the --kill-at argument to get undecorated exports from stdcall calling conventions
csharp-win: SWIG := ../../ThirdParty/swigwin-2.0.4/swig.exe
csharp-win: CSHARP_LINK_NATIVE_FLAGS += -static-libstdc++ -Wl,--subsystem,windows,--kill-at -Wl,--export-all-symbols
csharp-win: $(CSHARP_NATIVE_LIB_PACBIO)

#SWIG_DEBUG := -xmlout rr.xml
SWIG_DEBUG := 

$(CSHARP_NATIVE_LIB): $(CXX_LIB) $(SWIG_SRCS)
	mkdir -p $(BUILD_ROOT)/CSharp
	rm -rf $(BUILD_ROOT)/CSharp/*
	$(SWIG) $(SWIG_DEBUG) -csharp -dllimport libConsensusCore_ -namespace ConsensusCore -c++ -Isrc/C++ -outdir $(BUILD_ROOT)/CSharp \
	    -o $(BUILD_ROOT)/CSharp/ConsensusCore_wrap.cxx src/SWIG/ConsensusCore.i 
	$(CXX) $(CSHARP_INCLUDES) -c $(BUILD_ROOT)/CSharp/ConsensusCore_wrap.cxx -o $(BUILD_ROOT)/CSharp/ConsensusCore_wrap.o 
	$(CXX) $(CSHARP_LINK_NATIVE_FLAGS) $(BUILD_ROOT)/CSharp/ConsensusCore_wrap.o $(CXX_LIB) $(CSHARP_LIBS) -o $(BUILD_ROOT)/CSharp/libConsensusCore_.$(DYLIB)

$(CSHARP_CIL_LIB): $(CSHARP_NATIVE_LIB)
	cd $(BUILD_ROOT)/CSharp; $(CSHARP_COMPILE) -target:library -out:ConsensusCore.dll *.cs

$(CSHARP_NATIVE_LIB_PACBIO): $(CSHARP_NATIVE_LIB)
	mkdir -p $(PROJECT_ROOT)/csproj/bin/Debug/
	mkdir -p $(PROJECT_ROOT)/csproj/build/CSharp
	rm -rf $(PROJECT_ROOT)/csproj/build/CSharp/*
	cp $(CSHARP_NATIVE_LIB) $(CSHARP_NATIVE_LIB_PACBIO)
	cp $(BUILD_ROOT)/CSharp/*.cs $(PROJECT_ROOT)/csproj/build/CSharp

$(CSHARP_CIL_LIB_PACBIO): $(CSHARP_NATIVE_LIB_PACBIO) $(CSHARP_CIL_LIB)
	cp $(CSHARP_CIL_LIB) $(CSHARP_CIL_LIB_PACBIO)


csharp-demo:
	cd src/CSharp/; $(CSHARP_COMPILE) -lib:$(BUILD_ROOT)/CSharp -reference:ConsensusCore.dll Demo.cs -out:$(BUILD_ROOT)/CSharp/Demo.exe
	cd $(BUILD_ROOT)/CSharp; mono Demo.exe

clean-csharp:
	-rm -rf $(BUILD_ROOT)/CSharp
	-rm -rf $(PROJECT_ROOT)/csproj/bin
	-rm -rf $(PROJECT_ROOT)/csproj/build
	-rm -rf $(PROJECT_ROOT)/csproj/.depends
	-rm -rf $(PROJECT_ROOT)/csproj/PacBio.Analysis.ConsensusCoreManaged.mk

.PHONY: csharp csharp-pacbio csharp-win csharp-demo clean-csharp
