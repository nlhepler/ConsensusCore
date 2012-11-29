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

#include "Quiver/SseRecursor.hpp"

#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <climits>
#include <numeric>
#include <utility>

#include "Utils.hpp"
#include "Matrix/DenseMatrix.hpp"
#include "Matrix/SparseMatrix.hpp"
#include "Quiver/detail/Combiner.hpp"
#include "Quiver/QvEvaluator.hpp"
#include "Quiver/EdnaEvaluator.hpp"

using std::max;
using std::min;

#define NEG_INF   -FLT_MAX
#define POS_INF    FLT_MAX
#define NEG_INF_4  (Zero4<lfloat>())

namespace ConsensusCore {

    template<typename M, typename E, typename C>
    void
    SseRecursor<M, E, C>::FillAlpha(const E& e, const M& guide, M& alpha) const
    {
        int I = e.ReadLength();
        int J = e.TemplateLength();

        assert(alpha.Rows() == I + 1 && alpha.Columns() == J + 1);
        assert(guide.IsNull() ||
               (guide.Rows() == alpha.Rows() && guide.Columns() == alpha.Columns()));

        bool useGuide = !guide.IsNull();
        int hintBeginRow = 0, hintEndRow = 0;

        for (int j = 0; j <= J; ++j)
        {
            if (useGuide && !guide.IsColumnEmpty(j))
            {
                int guideBegin, guideEnd;
                boost::tie(guideBegin, guideEnd) = guide.UsedRowRange(j);
                hintBeginRow = min(hintBeginRow, guideBegin);
                hintEndRow   = max(hintEndRow, guideEnd);
            }

            int requiredEndRow = min(I + 1, hintEndRow);

            float score = NEG_INF;
            float thresholdScore = NEG_INF;
            float maxScore = NEG_INF;

            alpha.StartEditingColumn(j, hintBeginRow, hintEndRow);

            int i;
            int beginRow = hintBeginRow, endRow;
            // Handle beginning rows non-SSE.  Must handle row 0 this
            // way (if row 0 is to be filled), and must terminate with
            // (I - i + 1) divisible by 4, so that the SSE loop can
            // run safely to the end.  Banding optimizations not applied
            // here.
            //
            // TODO(dalexander): we could also handle the first two columns this way,
            // and then we could remove all the conditionals from the SSE loop.
            // Profile first.
            //
            for (i = beginRow;
                 (i == 0 || (I - i + 1) % 4 != 0) && i <= I;
                 i++)
            {
                score = NEG_INF;

                // Start:
                if (i == 0 && j == 0)
                {
                    score = 0.0f;
                }
                // Inc
                if (i > 0 && j > 0)
                {
                    score = C::Combine(score, alpha(i - 1, j - 1) + e.Inc(i - 1, j - 1));
                }
                // Merge
                if ((this->movesAvailable_ & MERGE) && (i > 0 && j > 1))
                {
                    score = C::Combine(score, alpha(i - 1, j - 2) + e.Merge(i - 1, j - 2));
                }
                // Delete
                if (j > 0)
                {
                    score = C::Combine(score, alpha(i, j - 1) + e.Del(i, j - 1));
                }
                // Extra
                if (i > 0)
                {
                    score = C::Combine(score, alpha(i - 1, j) + e.Extra(i - 1, j));
                }
                alpha.Set(i, j, score);

                if (score > maxScore)
                {
                    maxScore = score;
                    thresholdScore = maxScore - this->bandingOptions_.ScoreDiff;
                }
            }
            //
            // Main SSE loop
            //
            assert(i > 0);
            for (;
                 i <= I && (score >= thresholdScore || i < requiredEndRow);
                 i += 4)
            {
                __m128 score4 = NEG_INF_4;
                // Incorporation:
                if (j > 0)
                {
                    score4 = C::Combine4(score4, alpha.Get4(i - 1, j - 1) + e.Inc4(i - 1, j - 1));
                }
                // Merge
                if ((this->movesAvailable_ & MERGE) && j >= 2)
                {
                    score4 = C::Combine4(score4, alpha.Get4(i - 1, j - 2) + e.Merge4(i - 1, j - 2));
                }
                // Deletion:
                if (j > 0)
                {
                    score4 = C::Combine4(score4, alpha.Get4(i, j - 1) + e.Del4(i, j - 1));
                }

                alpha.Set4(i, j, score4);

                // Extra (non-SSE cascade)
                // (Meanwhile, set score to the minimum of score4, which will be used
                // to check for termination.)
                score = POS_INF;
                float potentialNewMax = NEG_INF;
                for (int ii = 0; ii < 4; ii++)
                {
                    float v = C::Combine(alpha(i + ii, j), alpha(i + ii - 1, j) +
                                                           e.Extra(i + ii - 1, j));
                    alpha.Set(i + ii, j, v);
                    score = min(score, v);
                    potentialNewMax = max(potentialNewMax, v);
                }

                if (potentialNewMax > maxScore)
                {
                    maxScore = potentialNewMax;
                    thresholdScore = maxScore - this->bandingOptions_.ScoreDiff;
                }
            }
            endRow = i;
            alpha.FinishEditingColumn(j, beginRow, endRow);

            // Now, revise the hints to tell the caller where the mass of the
            // distribution really lived in this column.
            hintEndRow = endRow;
            for (i = beginRow; i < endRow && alpha(i, j) < thresholdScore; ++i);
            hintBeginRow = i;
        }
    }


    template<typename M, typename E, typename C>
    void
    SseRecursor<M, E, C>::FillBeta(const E& e, const M& guide, M& beta) const
    {
        int I = e.ReadLength();
        int J = e.TemplateLength();

        assert(beta.Rows() == I + 1 && beta.Columns() == J + 1);
        assert(guide.IsNull() ||
               (guide.Rows() == beta.Rows() && guide.Columns() == beta.Columns()));

        bool useGuide = !guide.IsNull();
        int hintBeginRow = I + 1, hintEndRow = I + 1;

        for (int j = J; j >= 0; --j)
        {
            if (useGuide && !guide.IsColumnEmpty(j))
            {
                int guideBegin, guideEnd;
                boost::tie(guideBegin, guideEnd) = guide.UsedRowRange(j);
                hintBeginRow = min(hintBeginRow, guideBegin);
                hintEndRow   = max(hintEndRow, guideEnd);
            }

            int requiredBeginRow = max(0, hintBeginRow);

            float score = NEG_INF;
            float thresholdScore = NEG_INF;
            float maxScore = NEG_INF;

            beta.StartEditingColumn(j, hintBeginRow, hintEndRow);
            //
            // See comment in FillAlpha---we are doing the same thing here.
            // An initial non-SSE loop, terminating when a multiple of 4
            // rows remain.
            //
            int i, beginRow, endRow = hintEndRow;
            for (i = endRow - 1;
                 (i == I || (i + 1) % 4 != 0) && i >= 0;
                 i--)
            {
                score = NEG_INF;

                // Start:
                if (i == I && j == J)
                {
                    score = 0.0f;
                }
                // Inc
                if (i < I && j < J)
                {
                    score = C::Combine(score, beta(i + 1, j + 1) + e.Inc(i, j));
                }
                // Merge
                if ((this->movesAvailable_ & MERGE) && j < J - 1 && i < I)
                {
                    score = C::Combine(score, beta(i + 1, j + 2) + e.Merge(i, j));
                }
                // Delete
                if (j < J)
                {
                    score = C::Combine(score, beta(i, j + 1) + e.Del(i, j));
                }
                // Extra
                if (i < I)
                {
                    score = C::Combine(score, beta(i + 1, j) + e.Extra(i, j));
                }

                beta.Set(i, j, score);

                if (score > maxScore)
                {
                    maxScore = score;
                    thresholdScore = maxScore - this->bandingOptions_.ScoreDiff;
                }
            }
            //
            // SSE loop
            //
            i = i - 3;
            for (;
                 i >= 0 && (score >= thresholdScore || i >= requiredBeginRow);
                 i -= 4)
            {
                __m128 score4 = NEG_INF_4;

                // Incorporation:
                if (i < I && j < J)
                {
                    score4 = C::Combine4(score4, beta.Get4(i + 1, j + 1) + e.Inc4(i, j));
                }
                // Merge
                if ((this->movesAvailable_ & MERGE) && j < J - 1 && i < I)
                {
                    score4 = C::Combine4(score4, beta.Get4(i + 1, j + 2) + e.Merge4(i, j));
                }
                // Deletion:
                if (j < J)
                {
                    score4 = C::Combine4(score4, beta.Get4(i, j + 1) + e.Del4(i, j));
                }

                beta.Set4(i, j, score4);

                // Extra (non-SSE cascade)
                // (... and calculate min and max of score4)
                score = POS_INF;
                float potentialNewMax = NEG_INF;
                for (int ii = 3; ii >= 0; ii--)
                {
                    float v = C::Combine(beta(i + ii, j), beta(i + ii + 1, j) +
                                                          e.Extra(i + ii, j));
                    beta.Set(i + ii, j, v);
                    score = min(score, v);
                    potentialNewMax = max(potentialNewMax, v);
                }

                if (potentialNewMax > maxScore)
                {
                    maxScore = potentialNewMax;
                    thresholdScore = maxScore - this->bandingOptions_.ScoreDiff;
                }
            }

            beginRow = i + 4;
            beta.FinishEditingColumn(j, beginRow, endRow);

            // Now, revise the hints to tell the caller where the mass of the
            // distribution really lived in this column.
            hintBeginRow = beginRow;
            for (i = endRow;
                 i > beginRow && beta(i - 1, j) < thresholdScore;
                 i--);
            hintEndRow = i;
        }
    }

    template<typename M, typename E, typename C>
    float
    SseRecursor<M, E, C>::LinkAlphaBeta(const E& e,
                                        const M& alpha, int alphaColumn,
                                        const M& beta, int betaColumn,
                                        int absoluteColumn) const
    {
        const int I = e.ReadLength();

        assert(alphaColumn > 1 && absoluteColumn > 1);
        assert(absoluteColumn < e.TemplateLength());

        int usedBegin, usedEnd;

        std::pair<int, int> hull = std::make_pair(INT_MAX, 0);
        hull = RangeUnion(hull, alpha.UsedRowRange(alphaColumn - 2));
        hull = RangeUnion(hull, alpha.UsedRowRange(alphaColumn - 1));
        hull = RangeUnion(hull, beta.UsedRowRange(betaColumn));
        hull = RangeUnion(hull, beta.UsedRowRange(betaColumn + 1));
        boost::tie(usedBegin, usedEnd) = hull;

        float v = NEG_INF;
        __m128 v4 = NEG_INF_4;

        // SSE loop
        int i;
        for (i = usedBegin; i < usedEnd - 4; i += 4)
        {
            // Incorporate
            v4 = C::Combine4(v4, alpha.Get4(i, alphaColumn - 1) +
                                 e.Inc4(i, absoluteColumn - 1) +
                                 beta.Get4(i + 1, betaColumn));
            // Merge (2 possible ways):
            if (this->movesAvailable_ & MERGE)
            {
                v4 = C::Combine4(v4, alpha.Get4(i, alphaColumn - 2) +
                                     e.Merge4(i, absoluteColumn - 2) +
                                     beta.Get4(i + 1, betaColumn));
                v4 = C::Combine4(v4, alpha.Get4(i, alphaColumn - 1) +
                                     e.Merge4(i, absoluteColumn - 1) +
                                     beta.Get4(i + 1, betaColumn + 1));
            }
            // Delete
            v4 = C::Combine4(v4, alpha.Get4(i, alphaColumn - 1) +
                                 e.Del4(i, absoluteColumn - 1) +
                                 beta.Get4(i, betaColumn));
        }
        // Handle the remaining rows non-SSE
        for (; i < usedEnd; i++)
        {
            if (i < I)
            {
                // Incorporate
                v = C::Combine(v, alpha(i, alphaColumn - 1) +
                                  e.Inc(i, absoluteColumn - 1) +
                                  beta(i + 1, betaColumn));
                // Merge (2 possible ways):
                if (this->movesAvailable_ & MERGE)
                {
                    v = C::Combine(v, alpha(i, alphaColumn - 2) +
                                      e.Merge(i, absoluteColumn - 2) +
                                      beta(i + 1, betaColumn));
                    v = C::Combine(v, alpha(i, alphaColumn - 1) +
                                      e.Merge(i, absoluteColumn - 1) +
                                      beta(i + 1, betaColumn + 1));
                }
            }
            // Delete:
            v = C::Combine(v, alpha(i, alphaColumn - 1) +
                              e.Del(i, absoluteColumn - 1) +
                              beta(i, betaColumn));
        }
        // Combine v4 and v
        float v_array[5];
        _mm_storeu_ps(v_array, v4);
        v_array[4] = v;
        v = std::accumulate(v_array, v_array + 5, NEG_INF, C::Combine);
        return v;
    }

    template<typename M, typename E, typename C>
    void
    SseRecursor<M, E, C>::ExtendAlpha(const E& e,
                                      const M& alpha,
                                      int beginColumn,
                                      M& ext) const
    {
        assert(alpha.Rows() == e.ReadLength() + 1);
        // The new template may not be the same length as the old template.
        // Just make sure that we have anough room to fill out the extend buffer
        assert(beginColumn + 1 < e.TemplateLength() + 1);
        assert(ext.Rows() == e.ReadLength() + 1 && ext.Columns() == 2);
        assert (beginColumn >= 2);

        for (int extCol = 0; extCol < 2; extCol++)
        {
            int j = beginColumn + extCol;
            int beginRow, endRow;
            boost::tie(beginRow, endRow) = alpha.UsedRowRange(j);

            ext.StartEditingColumn(extCol, beginRow, endRow);
            int i;
            // Handle the first rows non-SSE, leaving a multiple of 4
            // entries to be handed off to the SSE loop.  Need to always
            // handle at least row 0 this way, so that we don't have
            // to check for (i > 0) in the SSE loop.
            for (i = beginRow;
                 (i == 0 || (endRow - i) % 4 != 0) && i < endRow;
                 i++)
            {
                float prev, score = NEG_INF;
                if (i > 0)
                {
                    // Inc
                    prev = (extCol == 0 ?
                                alpha(i - 1, j - 1) :
                                ext(i - 1, extCol - 1));
                    score = C::Combine(score, prev + e.Inc(i - 1, j - 1));
                    // Merge
                    if (this->movesAvailable_ & MERGE)
                    {
                        prev = alpha(i - 1, j - 2);
                        score = C::Combine(score, prev + e.Merge(i - 1, j - 2));
                    }
                }
                // Delete
                prev = (extCol == 0 ?
                            alpha(i, j - 1) :
                            ext(i, extCol - 1));
                score = C::Combine(score, prev + e.Del(i, j - 1));
                ext.Set(i, extCol, score);
            }
            for (; i < endRow - 3; i += 4)
            {
                __m128 prev4, score4 = NEG_INF_4;

                // Incorporation:
                prev4 = (extCol == 0 ?
                            alpha.Get4(i - 1, j - 1) :
                            ext.Get4(i - 1, extCol - 1));
                score4 = C::Combine4(score4, prev4 + e.Inc4(i - 1, j - 1));

                // Merge
                if ((this->movesAvailable_ & MERGE) && j >= 2)
                {
                    prev4 = alpha.Get4(i - 1, j - 2);
                    score4 = C::Combine4(score4, prev4 + e.Merge4(i - 1, j - 2));
                }

                // Deletion:
                prev4 = (extCol == 0 ?
                            alpha.Get4(i, j - 1) :
                            ext.Get4(i, extCol - 1));
                score4 = C::Combine4(score4, prev4 + e.Del4(i, j - 1));

                ext.Set4(i, extCol, score4);
            }
            assert (i == endRow);

            // Run back down the column and get the extras
            for (i = max(1, beginRow); i < endRow; i++)
            {
                float score = C::Combine(ext(i, extCol),
                                         ext(i - 1, extCol) + e.Extra(i - 1, j));
                ext.Set(i, extCol, score);
            }

            ext.FinishEditingColumn(extCol, beginRow, endRow);
        }
    }

    template<typename M, typename E, typename C>
    SseRecursor<M, E, C>::SseRecursor(int movesAvailable, const BandingOptions& banding)
        : detail::RecursorBase<M, E, C>(movesAvailable, banding)
    {}


    template class SseRecursor<DenseMatrix,  QvEvaluator, detail::ViterbiCombiner>;
    template class SseRecursor<SparseMatrix, QvEvaluator, detail::ViterbiCombiner>;
    template class SseRecursor<SparseMatrix, EdnaEvaluator, detail::SumProductCombiner>;
}

