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

#include <boost/range/as_array.hpp>
#include <vector>
#include <string>
#include <set>

#include <Types.hpp>
#include <Utils.hpp>
#include <Mutation.hpp>

#include "MutationEnumeration.hpp"

namespace ConsensusCore
{
    static const char BASES[] = { 'A', 'C', 'G', 'T' };

    std::vector<Mutation> AllMutations(std::string tpl, int beginPos, int endPos)
    {
        std::vector<Mutation> result;
        for (int pos = 0; pos < tpl.length(); pos++)
        {
            foreach (char base, boost::as_array(BASES)) {
                result.push_back(Mutation(INSERTION, pos, base));
            }
            result.push_back(Mutation(DELETION, pos, '-'));
            foreach (char base, boost::as_array(BASES)) {
                if (base != tpl[pos]) {
                    result.push_back(Mutation(SUBSTITUTION, pos, base));
                }
            }
        }
        return result;
    }

    std::vector<Mutation> AllMutations(std::string tpl)
    {
        return AllMutations(tpl, 0, tpl.length());
    }

    std::vector<Mutation> AllUniqueMutations(std::string tpl, int beginPos, int endPos)
    {
        std::vector<Mutation> result;
        for (int pos = beginPos; pos < endPos; pos++)
        {
            char prevTplBase = pos > 0 ? tpl[pos-1] : '-';
            // Insertions only allowed at the beginning of homopolymers
            foreach (char base, boost::as_array(BASES)) {
                if (base != prevTplBase) {
                    result.push_back(Mutation(INSERTION, pos, base));
                }
            }
            result.push_back(Mutation(DELETION, pos, '-'));
            foreach (char base, boost::as_array(BASES)) {
                if (base != tpl[pos]) {
                    result.push_back(Mutation(SUBSTITUTION, pos, base));
                }
            }
        }
        return result;
    }

    std::vector<Mutation> AllUniqueMutations(std::string tpl)
    {
        return AllUniqueMutations(tpl, 0, tpl.length());
    }

    ///
    /// Enumerate all mutations within a neighborhood of another set of
    /// mutations of interest.  Note that the neighborhoods are presently
    /// lopsided due to the end-exclusive definition for how we do ranges.
    /// (In other words a neighborhood of size 2 includes two before but one
    //   after).
    std::vector<Mutation> UniqueMutationsNearby(std::string tpl,
                                                std::vector<Mutation> centers,
                                                int neighborhoodSize)
    {
        std::set<Mutation> muts;
        foreach (const Mutation& center, centers)
        {
            int c = center.Start();
            int l = std::max(c - neighborhoodSize, 0);
            int r = std::min(c + neighborhoodSize, (int)tpl.length());
            std::vector<Mutation> mutsInRange = AllUniqueMutations(tpl, l, r);
            muts.insert(mutsInRange.begin(), mutsInRange.end());
        }
        std::vector<Mutation> result;
        std::copy(muts.begin(), muts.end(), back_inserter(result));
        return result;
    }
}
