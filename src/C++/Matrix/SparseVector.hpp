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
#include <utility>
#include <vector>

#include "LFloat.hpp"
#include "Types.hpp"
#include "Utils.hpp"

namespace ConsensusCore {

    class SparseVector
    {
    public:  // Constructor, destructor
        SparseVector(int logicalLength, int beginRow, int endRow);
        ~SparseVector();

        // Ensures there is enough allocated storage to
        // hold entries for at least [beginRow, endRow) (plus padding);
        // clears existing entries.
        void ResetForRange(int beginRow, int endRow);

    public:
        const float& operator()(int i) const;
        float Get(int i) const;
        void Set(int i, float v);
        __m128 Get4(int i) const;
        void Set4(int i, __m128 v);
        void Clear();

    public:
        int AllocatedEntries() const;
        void CheckInvariants() const;

    private:
        // Expand the range of rows for which we have backing storage,
        // while preserving contents.  The arguments will become the
        // new allocated bounds, so caller should add padding if desired
        // before calling.
        void ExpandAllocated(int newAllocatedBegin, int newAllocatedEnd);

    private:
        std::vector<float>* storage_;

        // the "logical" length of the vector, of which only
        // a subset of entries are actually allocated
        int logicalLength_;

        // row numbers in the abstraction we are presenting
        int allocatedBeginRow_;
        int allocatedEndRow_;

        // analytics
        int nReallocs_;
    };
}

#include "Matrix/SparseVector-inl.hpp"
