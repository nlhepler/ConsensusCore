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

// Author: Patrick Marks, David Alexander

#pragma once

#include <string>
#include <vector>

#include "Types.hpp"

namespace ConsensusCore
{
    enum MutationType
    {
        INSERTION, DELETION, SUBSTITUTION
    };

    /// \brief Single mutation to a template sequence.
    class Mutation
    {
    private:
        MutationType type_;
        int position_;
        char base_;

    public:
        Mutation(MutationType type, int position, char base);

        MutationType Type() const;
        bool IsSubstitution() const;
        bool IsInsertion() const;
        bool IsDeletion() const;

        /// \brief Template position of the mutation.
        /// If the mutation is an insertion, the understanding
        /// is that the new base is inserted before Position().
        int Position() const;

        char Base() const;

        int LengthDiff() const;
        std::string ToString() const;

    public:
        bool operator==(const Mutation& other) const;
        bool operator<(const Mutation& other) const;
    };


    std::string ApplyMutation(const Mutation& mut, const std::string& tpl);
    std::string ApplyMutations(const std::vector<Mutation*>& muts, const std::string& tpl);

    std::string MutationsToTranscript(const std::vector<Mutation*>& muts,
                                      const std::string& tpl);

    std::vector<int> TargetToQueryPositions(const std::vector<Mutation*>& mutations,
                                            const std::string& tpl);
}



