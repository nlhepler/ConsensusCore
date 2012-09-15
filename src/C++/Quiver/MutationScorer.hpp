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

#pragma once

#include <boost/noncopyable.hpp>
#include <string>

// TODO(dalexander): how can we remove this include??
//  We should move all template instantiations out to another
//  header, I presume.
#include "Quiver/SimpleRecursor.hpp"
#include "Quiver/SseRecursor.hpp"
#include "Types.hpp"
#include "Mutation.hpp"

namespace ConsensusCore
{
    template<typename R>
    class MutationScorer : private boost::noncopyable
    {
    public:
        typedef typename R::MatrixType    MatrixType;
        typedef typename R::EvaluatorType EvaluatorType;
        typedef R                         RecursorType;

    public:
        MutationScorer(const EvaluatorType& evaluator, const R& recursor);
        virtual ~MutationScorer();

    public:
        std::string Template() const;
        void Template(std::string tpl);
        float Score() const;
        float ScoreMutation(const Mutation& m) const;
        float ScoreMutation(MutationType mutationType, int position, char base) const;

    public:
        // Accessors that are handy for debugging.
        const MatrixType* Alpha() const;
        const MatrixType* Beta() const;
        const PairwiseAlignment* Alignment() const;
        const EvaluatorType* Evaluator() const;

    private:
        EvaluatorType* evaluator_;
        R* recursor_;
        MatrixType* alpha_;
        MatrixType* beta_;
        MatrixType* extendBuffer_;
    };

    typedef MutationScorer<SimpleQvRecursor>       SimpleQvMutationScorer;
    typedef MutationScorer<SseQvRecursor>          SseQvMutationScorer;
    typedef MutationScorer<SparseSimpleQvRecursor> SparseSimpleQvMutationScorer;
    typedef MutationScorer<SparseSseQvRecursor>    SparseSseQvMutationScorer;
    typedef MutationScorer<SparseSseEdnaRecursor>  SparseSseEdnaMutationScorer;
}
