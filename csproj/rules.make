
# Force build of the libPacBioNative_.so with a reference, rather than a build hook
pre-build-hook = make-native
make-native: $(TARGET_DIR)/libConsensusCore_.so


PacBio.Analysis.ConsensusCoreManaged.dll_PROJECT_REFERENCE_FILES += $(TARGET_DIR)/libConsensusCore_.so

# This is dummy rule to deal with the fact the SWIG C# files don't exist at the start of the make invocation
build/CSharp/*.cs:
	echo "Dummy rule for generated C# files" $*

$(TARGET_DIR)/libConsensusCore_.so: FORCE
	(cd ..; make csharp-pacbio)

FORCE:
