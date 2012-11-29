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

// Author: David Alexander

#include "Quiver/detail/RecursorBase.hpp"

#include <algorithm>
#include <boost/type_traits.hpp>
#include <string>
#include <vector>

#include "LFloat.hpp"
#include "Matrix/DenseMatrix.hpp"
#include "Matrix/SparseMatrix.hpp"
#include "PairwiseAlignment.hpp"
#include "Quiver/detail/Combiner.hpp"
#include "Quiver/EdnaEvaluator.hpp"
#include "Quiver/QuiverConfig.hpp"
#include "Quiver/QvEvaluator.hpp"
#include "Types.hpp"
#include "Utils.hpp"


// TODO(dalexander): put these into a RecursorConfig struct
#define MAX_FLIP_FLOPS                  5
#define ALPHA_BETA_MISMATCH_TOLERANCE   0.2

using std::max;
using std::min;


namespace ConsensusCore {
namespace detail {

    template<typename M, typename E, typename C>
    void
    RecursorBase<M, E, C>::FillAlphaBeta(const E& e, M& a, M& b) const
        throw(AlphaBetaMismatchException)
    {
        FillAlpha(e, M::Null(), a);
        FillBeta(e, a, b);

        int I = e.ReadLength();
        int J = e.TemplateLength();
        int flipflops = 0;

        while (fabs(a(I, J) - b(0, 0)) > ALPHA_BETA_MISMATCH_TOLERANCE
               && flipflops <= MAX_FLIP_FLOPS)
        {
            FillAlpha(e, b, a);
            FillBeta(e, a, b);
            flipflops++;
        }

        // TODO(dalexander): as a last ditch effort, we could increase the banding
        // TODO(dalexander): store histogram of # flip flops in this recursor, so we
        //  can get "recursor stats"

        if (fabs(a(I, J) - b(0, 0)) > ALPHA_BETA_MISMATCH_TOLERANCE)
        {
            // throw AlphaBetaMismatchException();
        }
    }

    struct MoveSpec {
        Move MoveType;
        int ReadDelta;
        int ReferenceDelta;
    };

    template<typename M, typename E, typename C>
    const PairwiseAlignment*
    RecursorBase<M, E, C>::Alignment(const E& e, const M& a) const
    {
        if (!boost::is_same<C, ViterbiCombiner>::value)
        {
            ShouldNotReachHere();
        }

        int I, J;
        I = e.ReadLength();
        J = e.TemplateLength();

        // Matrix must be filled in before requesting traceback
        assert(a(I, J) != lfloat());

        int i = I;
        int j = J;
        float pathScore = 0;

        MoveSpec incMove   = { INCORPORATE, 1, 1 };
        MoveSpec delMove   = { DELETE,      0, 1 };
        MoveSpec extraMove = { EXTRA,       1, 0 };
        MoveSpec mergeMove = { MERGE,       1, 2 };
        std::vector<MoveSpec> moves;

        while (i > 0 || j > 0)
        {
            MoveSpec bestMove = { INVALID_MOVE, 0, 0 };
            float prevScore, moveScore, totalScore;
            float bestScore = lfloat();
            float bestMoveScore = lfloat();

            if ((movesAvailable_ & BURST) && i > 0)
            {
                // Burst
                for (int hpLength = 1;
                     hpLength <= i && (moveScore = e.Burst(i - hpLength, j, hpLength)) > -FLT_MAX;
                     hpLength++)
                {
                    prevScore = a(i - hpLength, j);
                    float totalScore = prevScore + moveScore;
                    if (totalScore > bestScore)
                    {
                        MoveSpec burstMove = { BURST, hpLength, 0 };
                        bestMove = burstMove;
                        bestScore = totalScore;
                        bestMoveScore = moveScore;
                    }
                }
            }

            if (i > 0 && j > 0)
            {
                prevScore = a(i - 1, j - 1);
                moveScore = e.Inc(i - 1, j - 1);
                totalScore = prevScore + moveScore;
                if (totalScore > bestScore)
                {
                    // Incorporate (match or mismatch)
                    bestMove = incMove;
                    bestScore = totalScore;
                    bestMoveScore = moveScore;
                }
            }

            if (j > 0)
            {
                // Delete
                prevScore = a(i, j - 1);
                bool freeDelete = (!e.PinEnd() && i == I) || (!e.PinStart() && i == 0);
                moveScore = freeDelete ? 0 : e.Del(i, j - 1);
                totalScore = prevScore + moveScore;
                if (totalScore > bestScore)
                {
                    bestMove = delMove;
                    bestScore = totalScore;
                    bestMoveScore = moveScore;
                }
            }

            if (i > 0)
            {
                // Extra
                prevScore = a(i - 1, j);
                moveScore = e.Extra(i - 1, j);
                totalScore = prevScore + moveScore;
                if (totalScore > bestScore)
                {
                    bestMove = extraMove;
                    bestScore = totalScore;
                    bestMoveScore = moveScore;
                }
            }

            if ((movesAvailable_ & MERGE) && i > 0 && j > 1)
            {
                // Merge
                prevScore = a(i - 1, j - 2);
                moveScore = e.Merge(i - 1, j - 2);
                totalScore = prevScore + moveScore;
                if (totalScore > bestScore)
                {
                    bestMove = mergeMove;
                    bestScore = totalScore;
                    bestMoveScore = moveScore;
                }
            }
            assert(AlmostEqual(a(i, j), bestScore));
            assert(bestMove.MoveType != INVALID_MOVE);
            assert(bestMoveScore != lfloat());

            moves.push_back(bestMove);
            i -= bestMove.ReadDelta;
            j -= bestMove.ReferenceDelta;
            pathScore += bestMoveScore;
        }
        assert(i == 0 && j == 0);

        // Reverse moves
        std::reverse(moves.begin(), moves.end());

        // Replay moves and stringify.
        std::string target;
        std::string query;
        i = 0;
        j = 0;
        foreach (const MoveSpec& move, moves)
        {
            switch (move.MoveType) {
            case INCORPORATE:
                target     += e.Template()[j];
                query      += e.Read()[i];
                break;
            case EXTRA:
                target     += '-';
                query      += e.Read()[i];
                break;
            case DELETE:
                target     += e.Template()[j];
                query      += '-';
                break;
            case MERGE:
                target     += e.Template()[j];
                target     += e.Template()[j+1];
                query      += '-';
                query      += e.Read()[i];
                break;
            case BURST:
                for (int z = 0; z < move.ReadDelta; z++)
                {
                    target     += '-';
                    query      += e.Read()[i + z];
                }
                break;
            case INVALID_MOVE:
                ShouldNotReachHere();
                break;
            default:
                ShouldNotReachHere();
            }
            i += move.ReadDelta;
            j += move.ReferenceDelta;
            assert(target.length() == query.length());
        }

        return new PairwiseAlignment(target, query);
    }

    template<typename M, typename E, typename C>
    RecursorBase<M, E, C>::RecursorBase(int movesAvailable, const BandingOptions& bandingOptions)
        : movesAvailable_(movesAvailable),
          bandingOptions_(bandingOptions)
    {}

    template<typename M, typename E, typename C>
    RecursorBase<M, E, C>::~RecursorBase()
    {}

    // template instantiation
    template class RecursorBase<DenseMatrix, QvEvaluator, ViterbiCombiner>;
    template class RecursorBase<SparseMatrix, QvEvaluator, ViterbiCombiner>;
    template class RecursorBase<SparseMatrix, EdnaEvaluator, SumProductCombiner>;
}}
