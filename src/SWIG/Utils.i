%{
/* Includes the header in the wrapper code */
#include "Utils.hpp"
#include "Coverage.hpp"
using namespace ConsensusCore;
%}

%include "Types.hpp"

typedef unsigned int uint32_t;

#ifdef SWIGPYTHON
    %apply (int DIM1, uint32_t* IN_ARRAY1)
         { (int tStartDim, uint32_t* tStart),
           (int tEndDim,   uint32_t* tEnd)  };
    %apply (int DIM1, uint32_t* ARGOUT_ARRAY1)
         { (int winLen, uint32_t* coverage) };
#endif // SWIGPYTHON

%include "Utils.hpp"
%include "Coverage.hpp"
