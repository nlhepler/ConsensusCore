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
        return QvModelParams(-1.280691f,
                             -31.42568f,
                             -0.4476061f,
                             -15.58668f,
                             -0.4832615f,
                             -25.06574f,
                             -3.101073f,
                             -0.7184672f,
                             -18.f,
                             -1.707493f,
                             -44.34149f,
                             0.0f);
    }

    const QvModelParams QvModelParams::C2_130()
    {
        return QvModelParams(-1.41882f,
                             -6.58979f,
                             -0.366356f,
                             -1.22393f,
                             -0.30647f,
                             -3.26889f,
                             -0.899265f,
                             0.0403404f,
                             -0.377961f,
                             -0.328803f,
                             -2.65419f,
                             -0.28016f);
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
          MovesAvailable(movesAvailable),
          Banding(bandingOptions),
          FastScoreThreshold(fastScoreThreshold)
    {}
}
