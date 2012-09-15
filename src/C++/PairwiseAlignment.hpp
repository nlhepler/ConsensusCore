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

#include <string>
#include <vector>

namespace ConsensusCore {
    /// \brief A pairwise alignment
    class PairwiseAlignment {
    private:
        std::string target_;
        std::string query_;
        std::string transcript_;

    public:
        // target string, including gaps; usually the "reference"
        std::string Target() const;

        // query string, including gaps; usually the "read"
        std::string Query() const;

        // transcript as defined by Gusfield pg 215.
        std::string Transcript() const;

    public:
        float Accuracy() const;
        int Matches() const;
        int Errors() const;
        int Mismatches() const;
        int Insertions() const;
        int Deletions() const;
        int Length() const;

    public:
        PairwiseAlignment(const std::string& target,
                          const std::string& query);
    };

    //
    // We provide a crude Needleman-Wunsch implementations
    //  - no suppport for a subsitution matrix,
    //  - no support for affine gap penalties.

    struct NeedlemanWunschParams {
        float MatchScore;
        float MismatchScore;
        float InsertScore;
        float DeleteScore;

        NeedlemanWunschParams(float matchScore,
                              float mismatchScore,
                              float insertScore,
                              float deleteScore);
    };

    NeedlemanWunschParams DefaultNeedlemanWunschParams();

    PairwiseAlignment* Align(const std::string& target,
                             const std::string& query,
                             NeedlemanWunschParams params = DefaultNeedlemanWunschParams()); // NOLINT

    // These calls return an array, same len as target, containing indices into the query string.
    std::vector<int> TargetToQueryPositions(const std::string& transcript);
    std::vector<int> TargetToQueryPositions(const PairwiseAlignment& aln);
}
