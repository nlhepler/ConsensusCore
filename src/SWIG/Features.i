%{
/* Includes the header in the wrapper code */
#include "Feature.hpp"
#include "Features.hpp"
#include "Quiver/PBFeatures.hpp"
using namespace ConsensusCore;
%}

#if SWIGPYTHON

%include "numpy.i"
%numpy_typemaps(float, NPY_FLOAT, int)

%apply (float* IN_ARRAY1, int DIM1)
     { (const float* ptr, int length) };

#endif // SWIGPYTHON

%include "Feature.hpp"
%include "Features.hpp"
%include "Quiver/PBFeatures.hpp"

namespace ConsensusCore {
    %template(CharFeature) Feature<char>;
    %template(FloatFeature) Feature<float>;
	%template(IntFeature) Feature<int>;
}

%include "carrays.i"
%array_class(float, FloatArray);
%array_class(int, IntArray);

