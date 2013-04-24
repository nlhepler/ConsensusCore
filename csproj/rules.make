
# Force build of the libPacBioNative_.so with a reference, rather than a build hook
pre-build-hook = make-native
make-native: $(TARGET_DIR)/libConsensusCore_.so


PacBio.Analysis.ConsensusCoreManaged.dll_PROJECT_REFERENCE_FILES += $(TARGET_DIR)/libConsensusCore_.so

$(TARGET_DIR)/libConsensusCore_.so: FORCE
	(cd ..; make csharp-pacbio)

FORCE:
