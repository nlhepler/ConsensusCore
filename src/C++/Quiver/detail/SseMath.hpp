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

#include <xmmintrin.h>
#include <limits>

#include "Quiver/detail/sse_mathfun.h"


// todo: turn these into inline functions
#define ADD4(a, b) _mm_add_ps((a), (b))

#define AFFINE4(offset, slope, dataptr)                 \
  (_mm_add_ps(_mm_set_ps1(offset),                      \
              _mm_mul_ps(_mm_set_ps1(slope),            \
                         _mm_loadu_ps((dataptr)))))

#define MUX4(mask, a, b) (_mm_or_ps(_mm_and_ps((mask), (a)), _mm_andnot_ps((mask), (b))))

#define MAX4(a, b) _mm_max_ps((a), (b))


namespace ConsensusCore {
namespace detail {
    //
    // Log-space arithmetic
    //
    static const __m128 ones    = _mm_set_ps1(1.0f);

    inline __m128 logAdd4(__m128 aa, __m128 bb)
    {
        __m128 max = _mm_max_ps(aa, bb);
        __m128 min = _mm_min_ps(aa, bb);
        __m128 diff = _mm_sub_ps(min, max);
        return _mm_add_ps(max, log_ps(_mm_add_ps(ones, exp_ps(diff))));
    }

    inline float logAdd(float a, float b)
    {
        __m128 aa = _mm_set_ps1(a);
        __m128 bb = _mm_set_ps1(b);
        __m128 acc = logAdd4(aa, bb);

        // Use an aligned buf to extract
        ALIGN16_BEG float buf[4] ALIGN16_END;
        _mm_store_ps(&buf[0], acc);
        return buf[0];
    }
}}

