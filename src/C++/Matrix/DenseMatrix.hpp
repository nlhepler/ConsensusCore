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

#include <boost/numeric/ublas/matrix.hpp>
#include <utility>
#include <vector>

#include "LFloat.hpp"
#include "Types.hpp"
#include "Utils.hpp"

namespace ConsensusCore {

#ifndef SWIG
    // SWIG gags on this
    using boost::numeric::ublas::matrix;
    using boost::numeric::ublas::column_major;
#endif  // SWIG

    typedef matrix<lfloat, column_major> boost_dense_matrix;

    class DenseMatrix : private boost_dense_matrix
    {
    public:  // Constructor, destructor
        DenseMatrix(int rows, int cols);
        ~DenseMatrix();

    public:  // Nullability
        static const DenseMatrix& Null();
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
        int AllocatedEntries() const;  // an entry may be stored but not filled

    public:  // Accessors
        //
        // Note there is no non-const operator()(i, j) provided---for good
        // resaon.  We need to have set'ing go through a distinct code path
        // from get'ing, so that we can have safety checks in the set code
        // path.  This is not what you get with operator()(i,j) const +
        // non-const, since m(i,j) uses the operator according to the
        // constness/non-constness of m, not based on whether the operation
        // is a set or get.  To my knowledge, there is no way to do this in
        // C++.
        //
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
        std::vector<std::pair<int, int> > usedRanges_;
        int columnBeingEdited_;
        void CheckInvariants(int column) const;
    };
}

#include "Matrix/DenseMatrix-inl.hpp"
