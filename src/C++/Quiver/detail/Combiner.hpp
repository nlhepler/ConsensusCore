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

/// \file  Combiner.hpp
/// \brief Logic for how scores are combined when two alignment paths merge.

#include <algorithm>
#include "SseMath.hpp"
#include "Utils.hpp"

#pragma once

namespace ConsensusCore {
namespace detail {

    /// \brief A tag dispatch class calculating path-join score in the
    /// Viterbi recursion
    class ViterbiCombiner
    {
    public:
        static float Combine(float x, float y)
        {
            return std::max(x, y);
        }

        static __m128 Combine4(__m128 x4, __m128 y4)
        {
            return _mm_max_ps(x4, y4);
        }
    };

    /// \brief A tag dispatch class calculating path-join score in the
    /// Sum-Product recursion
    class SumProductCombiner
    {
    public:
        static float Combine(float x, float y)
        {
            return logAdd(x, y);
        }

        static __m128 Combine4(__m128 x4, __m128 y4)
        {
            return logAdd4(x4, y4);
        }
    };
}}

