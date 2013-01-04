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

// Author: David Alexander

#include "Quiver/QuiverConfig.hpp"

namespace ConsensusCore {

    const QvModelParams QvModelParams::Untrained()
    {
        QvModelParams p;
        p.Match            =  -1.280691;
        p.Mismatch         = -31.42568;
        p.MismatchS        =  -0.4476061;
        p.Branch           = -15.58668;
        p.BranchS          = -0.4832615;
        p.DeletionN        = -25.06574;
        p.DeletionWithTag  =  -3.101073;
        p.DeletionWithTagS =  -0.7184672;
        p.Nce              = -18;
        p.NceS             =  -1.707493;
        p.Merge            = -44.34149;
        p.MergeS           = 0.0;
        p.Burst            = -30;
        p.BurstS           =  -4;
        return p;
    }

    const QvModelParams QvModelParams::C2_130()
    {
        QvModelParams p;
        p.Match            = -1.41882;
        p.Mismatch         = -6.58979;
        p.MismatchS        = -0.366356;
        p.Branch           = -1.22393;
        p.BranchS          = -0.30647;
        p.DeletionN        = -3.26889;
        p.DeletionWithTag  = -0.899265;
        p.DeletionWithTagS =  0.0403404;
        p.Nce              = -0.377961;
        p.NceS             = -0.328803;
        p.Merge            = -2.65419;
        p.MergeS           = -0.28016;
        p.Burst            =  0;
        p.BurstS           =  0;
        return p;
    }

    const QvModelParams QvModelParams::Default()
    {
        return C2_130();
    }


    QuiverConfig::QuiverConfig(const QvModelParams& qvParams,
                               int movesAvailable,
                               const BandingOptions& bandingOptions,
                               float fastScoreThreshold)
        : QvParams(qvParams),
          Banding(bandingOptions),
          MovesAvailable(movesAvailable),
          FastScoreThreshold(fastScoreThreshold)
    {}
}
