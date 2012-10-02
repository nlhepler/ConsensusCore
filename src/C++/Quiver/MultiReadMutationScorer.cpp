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

#include <map>
#include <string>
#include <vector>

#include "Mutation.hpp"
#include "Quiver/MutationScorer.hpp"
#include "Quiver/MultiReadMutationScorer.hpp"
#include "Utils.hpp"

#define MARGIN 3

namespace ConsensusCore
{
    static bool readScoresPosition(const MappedRead* read, int position)
    {
        return (read->TemplateStart + MARGIN <= position &&
                read->TemplateEnd   - MARGIN > position);
    }

    template<typename R>
    MultiReadMutationScorer<R>::MultiReadMutationScorer(const R& recursor,
                                                        const typename R::EvaluatorType::ParamsType& params,  // NOLINT
                                                        std::string tpl)
        : recursor_(recursor),
          params_(params),
          fwdTemplate_(tpl),
          revTemplate_(ReverseComplement(tpl)),
          scorerForRead_()
    {
        DEBUG_ONLY(CheckInvariants());
    }

    template<typename R>
    MultiReadMutationScorer<R>::~MultiReadMutationScorer()
    {
        foreach (const item_t& kv, scorerForRead_)
        {
            delete kv.first;
            delete kv.second;
        }
    }

    template<typename R>
    int
    MultiReadMutationScorer<R>::TemplateLength() const
    {
        return fwdTemplate_.length();
    }

    template<typename R>
    std::string
    MultiReadMutationScorer<R>::Template(StrandEnum strand) const
    {
        return (strand == FORWARD_STRAND ? fwdTemplate_ : revTemplate_);
    }

    template<typename R>
    std::string
    MultiReadMutationScorer<R>::Template(StrandEnum strand,
                                         int templateStart,
                                         int templateEnd) const
    {
        int len = templateEnd - templateStart;
        if (strand == FORWARD_STRAND)
        {
            return fwdTemplate_.substr(templateStart, len);
        }
        else
        {
            return revTemplate_.substr(TemplateLength() - templateEnd, len);
        }
    }

    template<typename R>
    void
    MultiReadMutationScorer<R>::ApplyMutations(const std::vector<Mutation*>& mutations)
    {
        DEBUG_ONLY(CheckInvariants());
        std::vector<int> mtp = TargetToQueryPositions(mutations, fwdTemplate_);
        fwdTemplate_ = ConsensusCore::ApplyMutations(mutations, fwdTemplate_);
        revTemplate_ = ReverseComplement(fwdTemplate_);

        foreach (const item_t& kv, scorerForRead_)
        {
            int newTemplateStart = mtp[kv.first->TemplateStart];
            int newTemplateEnd   = mtp[kv.first->TemplateEnd];
            kv.second->Template(Template(kv.first->Strand,
                                         newTemplateStart,
                                         newTemplateEnd));
            kv.first->TemplateStart = newTemplateStart;
            kv.first->TemplateEnd   = newTemplateEnd;
        }
        DEBUG_ONLY(CheckInvariants());
    }

    template<typename R>
    void MultiReadMutationScorer<R>::AddRead(const QvSequenceFeatures& features,
                                             StrandEnum strand)
    {
        AddRead(features, strand, 0, TemplateLength());
    }

    template<typename R>
    void MultiReadMutationScorer<R>::AddRead(const QvSequenceFeatures & features,
                                             StrandEnum strand,
                                             int templateStart,
                                             int templateEnd)
    {
        DEBUG_ONLY(CheckInvariants());
        MappedRead* mr = new MappedRead(features, strand, templateStart, templateEnd);
        EvaluatorType ev(features, Template(strand, templateStart, templateEnd), params_);
        scorerForRead_[mr] = new MutationScorer<R>(ev, recursor_);
        DEBUG_ONLY(CheckInvariants());
    }

    template<typename R>
    void MultiReadMutationScorer<R>::AddRead(const MappedRead& mr)
    {
        DEBUG_ONLY(CheckInvariants());
        EvaluatorType ev(mr.Features,
                         Template(mr.Strand, mr.TemplateStart, mr.TemplateEnd),
                         params_);
        scorerForRead_[new MappedRead(mr)] = new MutationScorer<R>(ev, recursor_);
        DEBUG_ONLY(CheckInvariants());
    }

    static Mutation orientedMutation(const MappedRead* mr,
                                     const Mutation& mut)
    {
        if (mr->Strand == FORWARD_STRAND)
        {
            return Mutation(mut.Type(),
                            mut.Position() - mr->TemplateStart,
                            mut.Base());
        }
        else
        {
            // This is tricky business.  Insertions require an
            // adjustment to maintain the pre-base semantic.
            int pos = mr->TemplateEnd- 1 - mut.Position() \
                    + (mut.IsInsertion() ? 1 : 0);
            return Mutation(mut.Type(),
                            pos,
                            ComplementaryBase(mut.Base()));
        }
    }

    template<typename R>
    float MultiReadMutationScorer<R>::Score(const Mutation& m) const
    {
        float sum = 0;
        foreach (const item_t& kv, scorerForRead_)
        {
            if (readScoresPosition(kv.first, m.Position()))
            {
                Mutation orientedMut = orientedMutation(kv.first, m);
                sum += (kv.second->ScoreMutation(orientedMut) -
                        kv.second->Score());
            }
        }
        return sum;
    }

    template<typename R>
    float MultiReadMutationScorer<R>::FastScore(const Mutation& m) const
    {
        float sum = 0;
        foreach (const item_t& kv, scorerForRead_)
        {
            if (readScoresPosition(kv.first, m.Position()))
            {
                Mutation orientedMut = orientedMutation(kv.first, m);
                sum += (kv.second->ScoreMutation(orientedMut) -
                        kv.second->Score());
                // Hack alert: I need to replace this cut-out with
                // something more justifiable.  I am just checking it in
                // now to play with.
                if (sum < -500)
                {
                    return sum;
                }
            }
        }
        return sum;
    }

    template<typename R>
    std::vector<float> MultiReadMutationScorer<R>::Scores(const Mutation& m) const
    {
        std::vector<float> scoreByRead;
        foreach (const item_t& kv, scorerForRead_)
        {
            if (readScoresPosition(kv.first, m.Position()))
            {
                Mutation orientedMut = orientedMutation(kv.first, m);
                scoreByRead.push_back(kv.second->ScoreMutation(orientedMut) -
                                      kv.second->Score());
            }
        }
        return scoreByRead;
    }

    template<typename R>
    bool MultiReadMutationScorer<R>::IsFavorable(const Mutation& m) const
    {
        float sum = 0;
        foreach (const item_t& kv, scorerForRead_)
        {
            if (readScoresPosition(kv.first, m.Position()))
            {
                Mutation orientedMut = orientedMutation(kv.first, m);
                sum += (kv.second->ScoreMutation(orientedMut) -
                        kv.second->Score());
            }
        }
        return (sum > 0);
    }

    template<typename R>
    bool MultiReadMutationScorer<R>::FastIsFavorable(const Mutation& m) const
    {
        float sum = 0;
        foreach (const item_t& kv, scorerForRead_)
        {
            if (readScoresPosition(kv.first, m.Position()))
            {
                Mutation orientedMut = orientedMutation(kv.first, m);
                sum += (kv.second->ScoreMutation(orientedMut) -
                        kv.second->Score());
                // Hack alert: I need to replace this cut-out with
                // something more justifiable.  I am just checking it in
                // now to play with.
                if (sum < -500)
                {
                    return false;
                }
            }
        }
        return (sum > 0);
    }

    template<typename R>
    void MultiReadMutationScorer<R>::CheckInvariants() const
    {
#ifndef NDEBUG
        assert(revTemplate_ == ReverseComplement(fwdTemplate_));
        foreach (const item_t& kv, scorerForRead_)
        {
            assert((int)kv.second->Template().length() ==
                   kv.first->TemplateEnd - kv.first->TemplateStart);
            assert(kv.second->Template() == Template(kv.first->Strand,
                                                     kv.first->TemplateStart,
                                                     kv.first->TemplateEnd));
        }
#endif  // !NDEBUG
    }


    template class MultiReadMutationScorer<SparseSseQvRecursor>;
}
