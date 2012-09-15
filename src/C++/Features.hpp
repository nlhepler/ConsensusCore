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

#include <boost/range.hpp>
#include <boost/shared_array.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>

#include "Feature.hpp"
#include "Types.hpp"

namespace ConsensusCore
{
    /// \brief An object containing observed features from a sequencing run.
    class SequenceFeatures
    {
    public:
        explicit SequenceFeatures(const std::string& seq);
        int Length() const             { return sequence_.Length(); }
        Feature<char> Sequence() const { return sequence_; }

        /// Access to the sequence bases
        const char& operator[] (int i) const { return sequence_[i]; }
        char ElementAt(int i) const          { return (*this)[i]; }


    private:
        Feature<char> sequence_;
    };
}

