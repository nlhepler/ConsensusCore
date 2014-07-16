%{
#include "Mutation.hpp"
#include "Quiver/MutationEnumeration.hpp"
using namespace ConsensusCore;
%}

%include <std_pair.i>
%include <std_vector.i>

namespace std {
    %template(MutationVector)         std::vector<ConsensusCore::Mutation>;
    %template(ScoredMutationVector)  std::vector<ConsensusCore::ScoredMutation>;
};

%include "Mutation.hpp"
%include "Quiver/MutationEnumeration.hpp"
