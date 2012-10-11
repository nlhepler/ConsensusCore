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

#pragma once

#include <boost/noncopyable.hpp>
#include <string>
#include <vector>
#include <map>

#include "Types.hpp"
#include "Quiver/MappedRead.hpp"
#include "Quiver/MutationScorer.hpp"
#include "Quiver/QuiverConfig.hpp"
#include "Quiver/SseRecursor.hpp"

namespace ConsensusCore {

    template<typename R>
    class MultiReadMutationScorer : private boost::noncopyable
    {
    public:
        typedef R                                         RecursorType;
        typedef typename R::EvaluatorType                 EvaluatorType;
        typedef typename R::EvaluatorType::ParamsType     ParamsType;
        typedef typename ConsensusCore::MutationScorer<R> ScorerType;

    public:
        MultiReadMutationScorer(const R& recursor, const ParamsType& params, std::string tpl);
        ~MultiReadMutationScorer();

        int TemplateLength() const;
        int NumReads() const;

        std::string Template(StrandEnum strand = FORWARD_STRAND) const;
        std::string Template(StrandEnum strand, int templateStart, int templateEnd) const;
        void ApplyMutations(const std::vector<Mutation*>& mutations);

        // Reads provided must be clipped to the reference/scaffold window implied by the
        // template, however they need not span the window entirely---nonspanning reads
        // must be provided with (0-based) template start/end coordinates.
        void AddRead(const QvSequenceFeatures& features, StrandEnum strand);
        void AddRead(const QvSequenceFeatures& features, StrandEnum strand,
                     int templateStart, int templateEnd);
        void AddRead(const MappedRead& mappedRead);

        float Score(const Mutation& m) const;
        float FastScore(const Mutation& m) const;

        std::vector<float> Scores(const Mutation& m) const;

        bool IsFavorable(const Mutation& m) const;
        bool FastIsFavorable(const Mutation& m) const;

    public:
        // Alternate entry point for C# code, not requiring zillions of object
        // allocations.
        float Score(MutationType mutationType, int position, char base) const;

    private:
        void CheckInvariants() const;

    private:
        R recursor_;
        ParamsType params_;
        std::string fwdTemplate_;
        std::string revTemplate_;
        std::map<MappedRead*, ScorerType*> scorerForRead_;

        typedef std::map<MappedRead*, ScorerType*> map_t;
        typedef typename map_t::value_type         item_t;
    };

    typedef MultiReadMutationScorer<SparseSseQvRecursor> SparseSseQvMultiReadMutationScorer;
}
