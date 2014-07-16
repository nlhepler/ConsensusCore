%{
#include "Mutation.hpp"
#include "Quiver/MutationEnumeration.hpp"
using namespace ConsensusCore;
%}

%include <std_pair.i>
%include <std_vector.i>

namespace std {
    %template(MutationVector)         std::vector<ConsensusCore::Mutation>;
    %template(ScoredMutation)         std::pair<ConsensusCore::Mutation, float>;
    %template(ScoredMutationVector)   std::vector< std::pair<ConsensusCore::Mutation, float> >;
};

%include "Mutation.hpp"
%include "Quiver/MutationEnumeration.hpp"
