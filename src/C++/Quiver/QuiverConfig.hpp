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

#include "Utils.hpp"

namespace ConsensusCore
{
    enum Move
    {
        INVALID_MOVE = 0x0,
        INCORPORATE  = 0x1,
        EXTRA        = 0x2,
        DELETE       = 0x4,
        MERGE        = 0x8,
        BURST        = 0x10,
        BASIC_MOVES  = (INCORPORATE | EXTRA | DELETE),
        ALL_MOVES    = (BASIC_MOVES | MERGE)
    };

    /// \brief The banding optimizations to be used by a recursor
    struct BandingOptions
    {
        int DiagonalCross;
        float ScoreDiff;

        BandingOptions(int diagonalCross, float scoreDiff)
            : DiagonalCross(diagonalCross),
              ScoreDiff(scoreDiff)
        {}
    };


    /// \brief A parameter vector for analysis using the QV model
    struct QvModelParams
    {
        float Match;
        float Mismatch;
        float MismatchS;
        float Branch;
        float BranchS;
        float DeletionN;
        float DeletionWithTag;
        float DeletionWithTagS;
        float Nce;
        float NceS;
        float Merge;
        float MergeS;
        float Burst;
        float BurstS;

        // By providing a single constructor with default arguments,
        // we allow Python and other SWIG client languages to use
        // named parameters. However if the user forgets to set one of
        // the parameters, trouble!

        QvModelParams(float Match            = 0,
                      float Mismatch         = 0,
                      float MismatchS        = 0,
                      float Branch           = 0,
                      float BranchS          = 0,
                      float DeletionN        = 0,
                      float DeletionWithTag  = 0,
                      float DeletionWithTagS = 0,
                      float Nce              = 0,
                      float NceS             = 0,
                      float Merge            = 0,
                      float MergeS           = 0,
                      float Burst            = 0,
                      float BurstS           = 0)
            : Match(Match)
            , Mismatch(Mismatch)
            , MismatchS(MismatchS)
            , Branch(Branch)
            , BranchS(BranchS)
            , DeletionN(DeletionN)
            , DeletionWithTag(DeletionWithTag)
            , DeletionWithTagS(DeletionWithTagS)
            , Nce(Nce)
            , NceS(NceS)
            , Merge(Merge)
            , MergeS(MergeS)
            , Burst(Burst)
            , BurstS(BurstS)
        {}

        static const QvModelParams Untrained();
        static const QvModelParams C2_130();
        static const QvModelParams Default();
    };


    struct QuiverConfig
    {
        const QvModelParams QvParams;
        const int MovesAvailable;
        const BandingOptions Banding;
        const float FastScoreThreshold;

        QuiverConfig(const QvModelParams& qvParams,
                     int movesAvailable,
                     const BandingOptions& bandingOptions,
                     float fastScoreThreshold);
    };
}
