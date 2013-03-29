//
// Include this file *first* to pull in forward declarations.
//

%{
/* Includes the header in the wrapper code */
#include "Types.hpp"
using namespace ConsensusCore;
%}

%include <std_pair.i>
%include <std_string.i>
%include <std_vector.i>
%include "stdint.i"

%include "Types.hpp"

namespace std {
  %template(Interval)               std::pair<uint32_t, uint32_t>;
  %template(IntervalVector)         std::vector<Interval>;
  %template(IntVector)              std::vector<int>;
  %template(FloatVector)            std::vector<float>;
  %template(StringVector)           std::vector<string>;
  %template(FeaturesVector)         std::vector<const ConsensusCore::SequenceFeatures*>;
  %template(ScoredMutation)         std::pair<ConsensusCore::Mutation*, float>;
  %template(ScoredMutationVector)   std::vector< std::pair<ConsensusCore::Mutation*, float> >;
  %template(MutationVector)         std::vector<ConsensusCore::Mutation*>;
};
