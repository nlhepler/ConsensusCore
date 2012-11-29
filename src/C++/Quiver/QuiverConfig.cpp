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
