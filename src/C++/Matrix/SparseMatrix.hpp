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

#include "Matrix/SparseVector.hpp"
#include "Types.hpp"
#include "Utils.hpp"

namespace ConsensusCore {

    class SparseMatrix
    {
    public:  // Constructor, destructor
        SparseMatrix(int rows, int cols);
        ~SparseMatrix();

    public:  // Nullability
        static const SparseMatrix& Null();
        bool IsNull() const;

    public:  // Size information
        const int Rows() const;
        const int Columns() const;

    public:  // Information about entries filled by column
        void StartEditingColumn(int j, int hintBegin, int hintEnd);
        void FinishEditingColumn(int j, int usedBegin, int usedEnd);
        std::pair<int, int> UsedRowRange(int j) const;
        bool IsColumnEmpty(int j) const;
        int UsedEntries() const;
        int AllocatedEntries() const;  // an entry may be allocated but not used

    public:  // Accessors
        const float& operator()(int i, int j) const;
        float Get(int i, int j) const;
        void Set(int i, int j, float v);
        void ClearColumn(int j);

    public:  // SSE accessors, which access 4 successive entries in a column
        __m128 Get4(int i, int j) const;
        void Set4(int i, int j, __m128 v);

    public:
        // Method SWIG clients can use to get a native matrix (e.g. Numpy)
        // mat must be filled as a ROW major matrix
        void ToHostMatrix(float** mat, int* rows, int* cols) const;

    private:
        void CheckInvariants(int column) const;

    private:
        std::vector<SparseVector*> columns_;
        int nCols_;
        int nRows_;
        int columnBeingEdited_;
        std::vector<std::pair<int, int> > usedRanges_;
    };
}

#include "Matrix/SparseMatrix-inl.hpp"

