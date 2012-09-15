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

#include <algorithm>
#include <boost/foreach.hpp>
#include <string>

#include "Features.hpp"
#include "Quiver/PBFeatures.hpp"
#include "Types.hpp"
#include "Utils.hpp"


namespace
{
    void CheckTagFeature(ConsensusCore::Feature<float> feature)
    {
        foreach (const float& tag, feature)
        {
            if (!(tag == 'A' ||
                  tag == 'C' ||
                  tag == 'G' ||
                  tag == 'T' ||
                  tag == 'N' ||
                  tag ==  0))
            {
                throw ConsensusCore::InternalError(
                    "Invalid DelTag field---must be ASCII encoded.");
            }
        }
    }
}

namespace ConsensusCore
{
    QvSequenceFeatures::QvSequenceFeatures(const std::string& seq)
        : SequenceFeatures(seq),
          SequenceAsFloat(Length()),
          InsQv (Length()),
          SubsQv(Length()),
          DelQv (Length()),
          DelTag(Length()),
          MergeQv(Length())
    {
        for (int i = 0; i < Length(); i++)
        {
            SequenceAsFloat[i] = static_cast<float>(seq[i]);
        }
    }

    QvSequenceFeatures::QvSequenceFeatures(const std::string& seq,
                                           const float* insQv,
                                           const float* subsQv,
                                           const float* delQv,
                                           const float* delTag,
                                           const float* mergeQv)
        : SequenceFeatures(seq),
          SequenceAsFloat(Length()),
          InsQv (insQv, Length()),
          SubsQv(subsQv, Length()),
          DelQv (delQv, Length()),
          DelTag(delTag, Length()),
          MergeQv(mergeQv, Length())
    {
        for (int i = 0; i < Length(); i++)
        {
            SequenceAsFloat[i] = static_cast<float>(seq[i]);
        }
        CheckTagFeature(DelTag);
    }

    QvSequenceFeatures::QvSequenceFeatures(const std::string& seq,
                                           const Feature<float> insQv,
                                           const Feature<float> subsQv,
                                           const Feature<float> delQv,
                                           const Feature<float> delTag,
                                           const Feature<float> mergeQv)
        : SequenceFeatures(seq),
          SequenceAsFloat(Length()),
          InsQv (insQv),
          SubsQv(subsQv),
          DelQv (delQv),
          DelTag(delTag),
          MergeQv(mergeQv)
    {
        for (int i = 0; i < Length(); i++)
        {
            SequenceAsFloat[i] = static_cast<float>(seq[i]);
        }
        CheckTagFeature(DelTag);
    }

    ChannelSequenceFeatures::ChannelSequenceFeatures(const std::string& seq)
        : SequenceFeatures(seq),
          Channel(Length())
    {}

    ChannelSequenceFeatures::ChannelSequenceFeatures(const std::string& seq, const int* channel)
        : SequenceFeatures(seq),
          Channel(channel, Length())
    {}
}
