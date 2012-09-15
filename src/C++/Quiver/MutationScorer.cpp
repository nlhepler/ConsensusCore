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

// Author: Patrick Marks and David Alexander

#include "Quiver/MutationScorer.hpp"

#include <string>

#include "Matrix/DenseMatrix.hpp"
#include "Matrix/SparseMatrix.hpp"
#include "Quiver/QvEvaluator.hpp"
#include "Quiver/EdnaEvaluator.hpp"
#include "Quiver/SimpleRecursor.hpp"
#include "Quiver/SseRecursor.hpp"
#include "Mutation.hpp"

namespace ConsensusCore
{
    template<typename R>
    MutationScorer<R>::MutationScorer(const EvaluatorType& evaluator, const R& recursor)
        : evaluator_(new EvaluatorType(evaluator)),
          recursor_(new R(recursor))
    {
        // Allocate alpha and beta
        alpha_ = new MatrixType(evaluator.ReadLength() + 1,
                                evaluator.TemplateLength() + 1);
        beta_ = new MatrixType(evaluator.ReadLength() + 1,
                               evaluator.TemplateLength() + 1);
        // Buffer where we extend into
        extendBuffer_ = new MatrixType(evaluator.Read().size() + 1, 2);
        // Initial alpha and beta
        recursor.FillAlphaBeta(*evaluator_, *alpha_, *beta_);
    }

    template<typename R>
    float
    MutationScorer<R>::Score() const
    {
        return (*beta_)(0, 0);
    }

    template<typename R> std::string
    MutationScorer<R>::Template() const
    {
        return evaluator_->Template();
    }

    template<typename R>
    void MutationScorer<R>::Template(std::string tpl)
    {
        delete alpha_;
        delete beta_;
        evaluator_->Template(tpl);
        alpha_ = new MatrixType(evaluator_->ReadLength() + 1,
                       evaluator_->TemplateLength() + 1);
        beta_  = new MatrixType(evaluator_->ReadLength() + 1,
                       evaluator_->TemplateLength() + 1);
        recursor_->FillAlphaBeta(*evaluator_, *alpha_, *beta_);
    }

    template<typename R>
    const typename R::MatrixType* MutationScorer<R>::Alpha() const
    {
        return alpha_;
    }

    template<typename R>
    const typename R::MatrixType* MutationScorer<R>::Beta() const
    {
        return beta_;
    }

    template<typename R>
    const typename R::EvaluatorType* MutationScorer<R>::Evaluator() const
    {
        return evaluator_;
    }

    template<typename R>
    const PairwiseAlignment* MutationScorer<R>::Alignment() const
    {
        return recursor_->Alignment(*evaluator_, *alpha_);
    }

    template<typename R>
    float MutationScorer<R>::ScoreMutation(const Mutation& m) const
    {
        // For now, we cannot score mutations too close to the
        // boundaries of the template. For mutations by the bounds,
        // we will just return the unmutated score.
        if (m.Position() >= 3 && m.Position() <= (int)(Template().length()) - 3)
        {
            std::string oldTpl = evaluator_->Template();
            std::string newTpl = ApplyMutation(m, oldTpl);

            // The evaluator class needs to be rethought.  It is a bit
            // awkward the way we use it.  As Pat has pointed out, it
            // probably makes more sense to break out the template string
            // from the read data, as the way we operate is: change
            // template, check score, repeat---the read data is never
            // modified.
            evaluator_->Template(newTpl);

            int startCol = m.Position() - 1;
            int templateCol = m.Position();
            int betaCol = templateCol + 1 - m.LengthDiff();
            int alphaLinkCol = 2;

            recursor_->ExtendAlpha(*evaluator_, *alpha_, startCol, *extendBuffer_);
            float score = recursor_->LinkAlphaBeta(*evaluator_,
                                                   *extendBuffer_, alphaLinkCol,
                                                   *beta_, betaCol,
                                                   templateCol + 1);

            // Restore the original template.
            evaluator_->Template(oldTpl);
            return score;
        }
        else
        {
            return Score();
        }
    }

    template<typename R>
    float
    MutationScorer<R>::ScoreMutation(MutationType mutationType, int position, char base) const
    {
        Mutation v(mutationType, position, base);
        return ScoreMutation(v);
    }

    template<typename R>
    MutationScorer<R>::~MutationScorer()
    {
        delete extendBuffer_;
        delete beta_;
        delete alpha_;
        delete recursor_;
        delete evaluator_;
    }

    template class MutationScorer<SimpleQvRecursor>;
    template class MutationScorer<SseQvRecursor>;
    template class MutationScorer<SparseSimpleQvRecursor>;
    template class MutationScorer<SparseSseQvRecursor>;
    template class MutationScorer<SparseSseEdnaRecursor>;
}

