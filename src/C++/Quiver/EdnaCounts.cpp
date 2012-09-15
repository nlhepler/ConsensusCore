// Copyright (c) 2011, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// THIS SOFTWARE CONSTITUTES AND EMBODIES PACIFIC BIOSCIENCES' CONFIDENTIAL
// AND PROPRIETARY INFORMATION.
//
// Disclosure, redistribution and use of this software is subject to the
// terms and conditions of the applicable written agreement(s) between you
// and Pacific Biosciences, where "you" refers to you or your company or
// organization, as applicable.  Any other disclosure, redistribution or
// use is prohibited.
//
// THIS SOFTWARE IS PROVIDED BY PACIFIC BIOSCIENCES AND ITS CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: Patrick Marks

#include <xmmintrin.h>
#include <pmmintrin.h>
#include <cassert>
#include <cfloat>
#include <iostream>
#include <string>
#include <cmath>
#include <climits>
#include <utility>

#include "EdnaCounts.hpp"
#include "LFloat.hpp"
#include "Matrix/SparseMatrix.hpp"
#include "Quiver/detail/Combiner.hpp"
#include "Quiver/detail/RecursorBase.hpp"
#include "Quiver/EdnaEvaluator.hpp"
#include "Quiver/MutationScorer.hpp"
#include "Quiver/PBFeatures.hpp"
#include "Quiver/QuiverConfig.hpp"
#include "Quiver/SimpleRecursor.hpp"
#include "Types.hpp"
#include "Utils.hpp"

using std::min;
using std::max;

#define NEG_INF -FLT_MAX

namespace ConsensusCore
{
    void EdnaCounts::DoCount(Feature<int> channelRead,
                             EdnaEvaluator& eval,
                             MutationScorer<SparseSseEdnaRecursor>& scorer,
                             int j1, int j2, float *results)
    {
        const SparseMatrix *alpha = scorer.Alpha();
        const SparseMatrix *beta = scorer.Beta();

        int usedBegin, usedEnd;
        std::pair<int, int> hull = std::make_pair(INT_MAX, 0);
        hull = RangeUnion(hull, alpha->UsedRowRange(j1));
        hull = RangeUnion(hull, beta->UsedRowRange(j2));
        boost::tie(usedBegin, usedEnd) = hull;

        for (int k = 0; k < 5; k++)
            results[k] = NEG_INF;

        for (int i = usedBegin; i < usedEnd; i++)
        {
            results[0] = detail::logAdd(results[0],
                                        alpha->Get(i, j1) +
                                        eval.ScoreMove(j1, j2, 0) +
                                        beta->Get(i, j2));
        }

        int nRows = alpha->Rows();
        int usedCap = usedEnd < nRows - 1 ? usedEnd : nRows - 1;

        for (int i = usedBegin; i < usedCap; i++)
        {
            int readBase = channelRead[i];
            results[readBase] = detail::logAdd(results[readBase],
                                               alpha->Get(i, j1) +
                                               eval.ScoreMove(j1, j2, readBase) +
                                               beta->Get(i+1, j2));
        }
    }
}
