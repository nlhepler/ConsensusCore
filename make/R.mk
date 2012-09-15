R_COMPILE    := R CMD SHLIB
R_LIB        := $(BUILD_ROOT)/R/ConsensusCore.so
R_STUB       := $(BUILD_ROOT)/R/ConsensusCore.R

# GNU sed is needed to get "-i" (in place modification)
GSED         := gsed

R: $(R_LIB)

$(R_LIB): $(CXX_LIB) $(SWIG_SRCS)
	mkdir -p $(BUILD_ROOT)/R
	$(SWIG) -r -c++ -Isrc/C++ -outdir $(BUILD_ROOT)/R \
		-o $(BUILD_ROOT)/R/ConsensusCore_wrap.cpp src/SWIG/ConsensusCore.i  

        # R apparently cannot handle variables beyond 256 bytes in
        # length, which is a problem for some of the massive
        # fully-qualified templated type names in ConsensusCore.  We
        # work around this by abbreviating ConsensusCore to CC---which
        # we have to do as a post-pass with sed, since there doesn't
        # seem to be a way to tell SWIG to do it for us.  --DHA
	$(GSED) -i  s/p_ConsensusCore/p_CC/g $(R_STUB)
	$(GSED) -i  s/ConsensusCore::/CC::/g $(R_STUB)

	PKG_CXXFLAGS="-Isrc/C++ -I$(BOOST_INCLUDE_PATH)" \
	PKG_LIBS="$(CXX_LIB)"                            \
	$(R_COMPILE) -o $(R_LIB) $(BUILD_ROOT)/R/ConsensusCore_wrap.cpp

clean-R:
	rm -rf $(BUILD_ROOT)/R

.PHONY: R clean-R