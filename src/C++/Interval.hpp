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

#pragma once

#include <algorithm>
#include <utility>

namespace ConsensusCore {

    class Interval : public std::pair<int, int>
    {
    public:
        Interval() : std::pair<int, int>() {}
        Interval(const int& a, const int& b) : std::pair<int, int>(a, b) {}

        inline
        Interval& operator=(const std::pair<int, int>& p)
        {
            std::pair<int, int>::operator=(p);
            return *this;
        }
    };

    inline Interval
    RangeUnion(const Interval& range1, const Interval& range2)
    {
        return Interval(std::min(range1.first, range2.first),
                        std::max(range1.second, range2.second));
    }

    inline Interval
    RangeUnion(const Interval& range1, const Interval& range2,
               const Interval& range3)
    {
        return RangeUnion(range1, RangeUnion(range2, range3));
    }

    inline Interval
    RangeUnion(const Interval& range1, const Interval& range2,
               const Interval& range3, const Interval& range4)
    {
        return RangeUnion(RangeUnion(range1, range2),
                          RangeUnion(range3, range4));
    }
}
