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
#include "Features.hpp"

namespace ConsensusCore
{
    /// \brief A features object that contains PulseToBase QV metrics
    struct QvSequenceFeatures : public SequenceFeatures
    {
        Feature<float> SequenceAsFloat;
        Feature<float> InsQv;
        Feature<float> SubsQv;
        Feature<float> DelQv;
        Feature<float> DelTag;
        Feature<float> MergeQv;

        explicit QvSequenceFeatures(const std::string& seq);

        QvSequenceFeatures(const std::string& seq,
                           const float* insQv,
                           const float* subsQv,
                           const float* delQv,
                           const float* delTag,
                           const float* mergeQv);

        QvSequenceFeatures(const std::string& seq,
                           const Feature<float> insQv,
                           const Feature<float> subsQv,
                           const Feature<float> delQv,
                           const Feature<float> delTag,
                           const Feature<float> mergeQv);
    };


    /// \brief A features object that contains sequence in channel space.
    struct ChannelSequenceFeatures : SequenceFeatures
    {
        Feature<int> Channel;

        explicit ChannelSequenceFeatures(const std::string& seq);

        ChannelSequenceFeatures(const std::string& seq, const int* channel);
    };
}
