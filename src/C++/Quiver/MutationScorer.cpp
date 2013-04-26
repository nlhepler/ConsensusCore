// Copyright (c) 2011-2013, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

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
        extendBuffer_ = new MatrixType(evaluator.ReadLength() + 1, 8);
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
    float
    MutationScorer<R>::ScoreMutation(MutationType mutationType, int position, char base) const
    {
        Mutation m(mutationType, position, base);
        return ScoreMutation(m);
    }

    template<typename R>
    float
    MutationScorer<R>::ScoreMutation(MutationType mutationType, int start, int end, std::string newBases) const
    {
        Mutation m(mutationType, start, end, newBases);
        return ScoreMutation(m);
    }

    template<typename R>
    float
    MutationScorer<R>::ScoreMutation(const Mutation& m) const
    {
        int betaLinkCol = 1 + m.End();
        int absoluteLinkColumn = 1 + m.End() + m.LengthDiff();

        if (m.Start() > 0 && m.End() < (int)(Template().length()) - 1)
        {
            // Install mutated template
            std::string oldTpl = evaluator_->Template();
            std::string newTpl = ApplyMutation(m, oldTpl);
            evaluator_->Template(newTpl);

            float score;
            if (m.Type() == DELETION)
            {
                // If we revise the semantic of Extra, we can remove the extend and just
                // link alpha and beta directly.
                int extendStartCol = m.Start() - 1;
                int extendLength = 2;
                recursor_->ExtendAlpha(*evaluator_, *alpha_, extendStartCol, *extendBuffer_, extendLength);
                score = recursor_->LinkAlphaBeta(*evaluator_,
                                                 *extendBuffer_, extendLength,
                                                 *beta_, betaLinkCol,
                                                 absoluteLinkColumn);
            }
            else
            {
                int extendStartCol = m.Start();
                int extendLength   = 1 + m.NewBases().length();
                assert(extendLength <= 8);

                recursor_->ExtendAlpha(*evaluator_, *alpha_, extendStartCol, *extendBuffer_, extendLength);
                score = recursor_->LinkAlphaBeta(*evaluator_,
                                                 *extendBuffer_, extendLength,
                                                 *beta_, betaLinkCol,
                                                 absoluteLinkColumn);
            }
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
