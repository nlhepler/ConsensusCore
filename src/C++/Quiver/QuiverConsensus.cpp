// Copyright (c) 2011-2014, Pacific Biosciences of California, Inc.
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

// Author: David Alexander

#include <algorithm>
#include <utility>
#include <boost/tuple/tuple.hpp>

#include "Quiver/MultiReadMutationScorer.hpp"
#include "Quiver/QuiverConsensus.hpp"
#include "Quiver/MutationEnumeration.hpp"

#include "Utils.hpp"

// hook these up to something
#define LOG_WARN(msg)
#define LOG_INFO(msg)

#define MAX_ROUNDS       20
#define MUT_SEPARATION   7
#define MUT_NEIGHBORHOOD 12

namespace ConsensusCore
{
    using std::vector;
    using std::make_pair;
    using std::max_element;
    using std::transform;

    static vector<ScoredMutation>
    DeleteRange(vector<ScoredMutation> input, int rStart, int rEnd)
    {
        vector<ScoredMutation> output;
        foreach (ScoredMutation s, input)
        {
            int pos = s.first.Start();
            if (!(rStart <= pos &&  pos <= rEnd))
            {
                output.push_back(s);
            }
        }
        return output;
    }

    static bool ScoreComparer(ScoredMutation i, ScoredMutation j)
    {
        return i.second < j.second;
    }

    //    Given a list of (mutation, score) tuples, this utility method
    //    greedily chooses the highest scoring well-separated elements.  We
    //    use this to avoid applying adjacent high scoring mutations, which
    //    are the rule, not the exception.  We only apply the best scoring one
    //    in each neighborhood, and then revisit the neighborhoods after
    //    applying the mutations.
    //
    //    This is highly unoptimized.  It is not in the critical path.
    vector<ScoredMutation>
    BestSubset(vector<ScoredMutation> input, int mutationSeparation)
    {
        vector<ScoredMutation> output;

        while (!input.empty())
        {
            ScoredMutation& best = *max_element(input.begin(), input.end(), ScoreComparer);
            output.push_back(best);
            int nStart = best.first.Start() - mutationSeparation;
            int nEnd = best.first.Start() + mutationSeparation;
            input = DeleteRange(input, nStart, nEnd);
        }

        return output;
    }


    bool QuiverConsensus::IterateToConsensus(AbstractMultiReadMutationScorer& mms)
    {
        vector<Mutation> favorableMuts;
        vector<ScoredMutation> favorableMutsAndScores;

        bool isConverged = false;
        for (int round = 1; round <= MAX_ROUNDS; round++)
        {
            //
            // Try all mutations in round 1.  In subsequent rounds, try mutations
            // nearby those used in previous round.
            //
            vector<Mutation> mutationsToTry;
            if (round == 1) {
                mutationsToTry = AllUniqueMutations(mms.Template());
            } else {
                mutationsToTry = UniqueMutationsNearby(mms.Template(), mutationsToTry, MUT_NEIGHBORHOOD);
            }

            //
            // Screen for favorable mutations.  If none, we are done (converged).
            //
            foreach (const Mutation& m, mutationsToTry)
            {
                if (mms.FastIsFavorable(m)) {
                    float score = mms.Score(m);
                    favorableMutsAndScores.push_back(make_pair(m, score));
                }
            }
            if (favorableMutsAndScores.empty())
            {
                isConverged = true;
                break;
            }

            //
            // Go with the "best" subset of well-separated high scoring mutations
            //
            vector<ScoredMutation> bestSubset = BestSubset(favorableMutsAndScores, MUT_SEPARATION);
            vector<Mutation> bestMutations;
            foreach (const ScoredMutation& ms, bestSubset)
            {
                bestMutations.push_back(ms.first);
            }
            mms.ApplyMutations(bestMutations);
        }

        return isConverged;
    }
}
