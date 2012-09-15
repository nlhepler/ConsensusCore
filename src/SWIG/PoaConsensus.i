
%{
/* Includes the header in the wrapper code */
#include <Poa/PoaConfig.hpp>
#include <Poa/PoaGraph.hpp>
#include <Poa/PoaConsensus.hpp>
using namespace ConsensusCore;
%}


%include <Poa/PoaConfig.hpp>
%include <Poa/PoaGraph.hpp>

%newobject *::FindConsensus(const std::vector<std::string>& reads, const PoaConfig& config);
%newobject *::FindConsensus(const std::vector<std::string>& reads, bool global);
%newobject *::FindConsensus(const std::vector<std::string>& reads);

%include <Poa/PoaConsensus.hpp>
