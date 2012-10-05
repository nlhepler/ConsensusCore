%{
/* Includes the header in the wrapper code */
#include <PairwiseAlignment.hpp>
#include <AffineAlignment.hpp>
using namespace ConsensusCore;
%}

%include "Types.i"

%newobject Align;
%newobject AlignWithAffineGapPenalty;

%include <PairwiseAlignment.hpp>
%include <AffineAlignment.hpp>


