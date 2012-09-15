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

#include "Quiver/SimpleRecursor.hpp"

#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <climits>
#include <utility>

#include "Matrix/DenseMatrix.hpp"
#include "Matrix/SparseMatrix.hpp"
#include "Quiver/detail/Combiner.hpp"
#include "Quiver/detail/RecursorBase.hpp"
#include "Quiver/QvEvaluator.hpp"
#include "Utils.hpp"

using std::min;
using std::max;

#define NEG_INF -FLT_MAX

namespace ConsensusCore {

    template<typename M, typename E, typename C>
    void
    SimpleRecursor<M, E, C>::FillAlpha(const E& e, const M& guide, M& alpha) const
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

            int i;
            float score = NEG_INF;
            float thresholdScore = NEG_INF;
            float maxScore = NEG_INF;

            alpha.StartEditingColumn(j, hintBeginRow, hintEndRow);

            int beginRow = hintBeginRow, endRow;
            for (i = beginRow;
                 i < I + 1 && (score >= thresholdScore || i < requiredEndRow);
                 ++i)
            {
                float thisMoveScore;
                score = NEG_INF;

                // Start:
                if (i == 0 && j == 0)
                {
                    score = 0.0f;
                }

                // Incorporation:
                if (i > 0 && j > 0)
                {
                    thisMoveScore = alpha(i - 1, j - 1) + e.Inc(i - 1, j - 1);
                    score = C::Combine(score, thisMoveScore);
                }

                // Extra:
                if (i > 0)
                {
                    thisMoveScore = alpha(i - 1, j) + e.Extra(i - 1, j);
                    score = C::Combine(score, thisMoveScore);
                }

                // Delete:
                if (j > 0)
                {
                    thisMoveScore = alpha(i, j - 1) + e.Del(i, j - 1);
                    score = C::Combine(score, thisMoveScore);
                }

                // Merge:
                if ((this->movesAvailable_ & MERGE) && j > 1 && i > 0)
                {
                    thisMoveScore = alpha(i - 1, j - 2) + e.Merge(i - 1, j - 2);
                    score = C::Combine(score, thisMoveScore);
                }

                // Burst:
                if (this->movesAvailable_ & BURST)
                {
                    NotYetImplemented();
                }

                //  Save score
                alpha.Set(i, j, score);

                if (score > maxScore)
                {
                    maxScore = score;
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
    SimpleRecursor<M, E, C>::FillBeta(const E& e, const M& guide, M& beta) const
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

            beta.StartEditingColumn(j, hintBeginRow, hintEndRow);

            int i;
            float score = NEG_INF;
            float thresholdScore = NEG_INF;
            float maxScore = NEG_INF;

            int beginRow, endRow = hintEndRow;
            for (i = endRow - 1;
                 i >= 0 && (score >= thresholdScore || i >= requiredBeginRow);
                 --i)
            {
                float thisMoveScore;
                score = NEG_INF;

                // Start:
                if (i == I && j == J)
                {
                    score = 0.0f;
                }

                // Incorporation:
                if (i < I && j < J)
                {
                    thisMoveScore = beta(i + 1, j + 1) + e.Inc(i, j);
                    score = C::Combine(score, thisMoveScore);
                }

                // Extra:
                if (i < I)
                {
                    thisMoveScore = beta(i + 1, j) + e.Extra(i, j);
                    score = C::Combine(score, thisMoveScore);
                }

                // Delete:
                if (j < J)
                {
                    thisMoveScore = beta(i, j + 1) + e.Del(i, j);
                    score = C::Combine(score, thisMoveScore);
                }

                // Merge:
                if ((this->movesAvailable_ & MERGE) && j < J - 1 && i < I)
                {
                    thisMoveScore = beta(i + 1, j + 2) + e.Merge(i, j);
                    score = C::Combine(score, thisMoveScore);
                }

                // Burst:
                if (this->movesAvailable_ & BURST)
                {
                    NotYetImplemented();
                }

                //  Save score
                beta.Set(i, j, score);

                if (score > maxScore)
                {
                    maxScore = score;
                    thresholdScore = maxScore - this->bandingOptions_.ScoreDiff;
                }
            }
            beginRow = i + 1;
            beta.FinishEditingColumn(j, beginRow, endRow);

            // Now, revise the hints to tell the caller where the mass of the
            // distribution really lived in this column.
            hintBeginRow = beginRow;
            for (i = endRow;
                 i > beginRow && beta(i - 1, j) < thresholdScore;
                 --i);
            hintEndRow = i;
        }
    }

    /// Calculate the recursion score by "stitching" together partial
    /// alpha and beta matrices.  alphaColumn, betaColumn, and
    /// absoluteColumn all refer to the same logical position in the
    /// template, but may have different values if, for instance,
    /// alpha here is a sub-range of the columns of the full alpha
    /// matrix.  Columns betaColumn and betaColumn + 1 of beta will be
    /// read; columns alphaColumn - 1 and alphaColumn - 2 of alpha
    /// will be read.
    template<typename M, typename E, typename C>
    float
    SimpleRecursor<M, E, C>::LinkAlphaBeta(const E& e,
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

        float v = NEG_INF, thisMoveScore;

        for (int i = usedBegin; i < usedEnd; i++)
        {
            if (i < I)
            {
                // Incorporate
                thisMoveScore = alpha(i, alphaColumn - 1) +
                                e.Inc(i, absoluteColumn - 1) +
                                beta(i + 1, betaColumn);
                v = C::Combine(v, thisMoveScore);

                // Merge (2 possible ways):
                thisMoveScore = alpha(i, alphaColumn - 2) +
                                e.Merge(i, absoluteColumn - 2) +
                                beta(i + 1, betaColumn);
                v = C::Combine(v, thisMoveScore);

                thisMoveScore = alpha(i, alphaColumn - 1) +
                                e.Merge(i, absoluteColumn - 1) +
                                beta(i + 1, betaColumn + 1);
                v = C::Combine(v, thisMoveScore);
            }

            // Delete:
            thisMoveScore = alpha(i, alphaColumn - 1) +
                            e.Del(i, absoluteColumn - 1) +
                            beta(i, betaColumn);
            v = C::Combine(v, thisMoveScore);
        }

        return v;
    }


    template<typename M, typename E, typename C>
    void
    SimpleRecursor<M, E, C>::ExtendAlpha(const E& e,
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
            float score = NEG_INF;
            float thresholdScore = NEG_INF;
            float maxScore = NEG_INF;

            for (i = beginRow; i < endRow; i++)
            {
                float thisMoveScore;
                maxScore = thresholdScore = score = NEG_INF;

                // Start:
                if (i == 0 && j == 0)
                {
                    score = 0.0f;
                }

                // Incorporation:
                if (i > 0 && j > 0)
                {
                    float prev = extCol == 0 ?
                            alpha(i - 1, j - 1) :
                            ext(i - 1, extCol - 1);
                    thisMoveScore = prev + e.Inc(i - 1, j - 1);
                    score = C::Combine(score, thisMoveScore);
                }

                // Extra:
                if (i > 0)
                {
                    thisMoveScore = ext(i - 1, extCol) + e.Extra(i - 1, j);
                    score = C::Combine(score, thisMoveScore);
                }

                // Delete:
                if (j > 0)
                {
                    float prev = extCol == 0 ?
                            alpha(i, j - 1) :
                            ext(i, extCol - 1);
                    thisMoveScore = prev + e.Del(i, j - 1);
                    score = C::Combine(score, thisMoveScore);
                }

                // Merge:
                if ((this->movesAvailable_ & MERGE) && j > 1 && i > 0)
                {
                    float prev = alpha(i - 1, j - 2);
                    thisMoveScore = prev + e.Merge(i - 1, j - 2);
                    score = C::Combine(score, thisMoveScore);
                }

                // Burst:
                if (this->movesAvailable_ & BURST)
                {
                    NotYetImplemented();
                }

                ext.Set(i, extCol, score);
            }
            assert (i == endRow);
            ext.FinishEditingColumn(extCol, beginRow, endRow);
        }
    }


    template<typename M, typename E, typename C>
    SimpleRecursor<M, E, C>::SimpleRecursor()
        : detail::RecursorBase<M, E, C>()
    {}


    template<typename M, typename E, typename C>
    SimpleRecursor<M, E, C>::SimpleRecursor(int movesAvailable, const BandingOptions& banding)
        : detail::RecursorBase<M, E, C>(movesAvailable, banding)
    {}


    template class SimpleRecursor<DenseMatrix,  QvEvaluator, detail::ViterbiCombiner>;
    template class SimpleRecursor<SparseMatrix, QvEvaluator, detail::ViterbiCombiner>;
}


