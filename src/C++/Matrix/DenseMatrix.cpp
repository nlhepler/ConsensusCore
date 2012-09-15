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


#include "Matrix/DenseMatrix.hpp"

#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <cassert>

#include "LFloat.hpp"

using boost::numeric::ublas::matrix;
using boost::numeric::ublas::row_major;

namespace ConsensusCore {

    // Performance insensitive routines are not inlined

    DenseMatrix::DenseMatrix(int rows, int cols)
        : boost_dense_matrix(rows, cols),
          usedRanges_(cols, std::make_pair(0, 0)),
          columnBeingEdited_(-1)
    {
        for (int j = 0; j < cols; j++)
        {
            CheckInvariants(j);
        }
    }

    DenseMatrix::~DenseMatrix()
    {}

    int
    DenseMatrix::UsedEntries() const
    {
        // use column ranges
        int filledEntries = 0;
        for (int col = 0; col < Columns(); ++col)
        {
            int start, end;
            boost::tie(start, end) = UsedRowRange(col);
            filledEntries += (end - start);
        }
        return filledEntries;
    }

    int
    DenseMatrix::AllocatedEntries() const
    {
        return Rows() * Columns();
    }

    void
    DenseMatrix::ToHostMatrix(float** mat, int* rows, int* cols) const
    {
        // TODO(dalexander): make sure SWIG client deallocates this memory -- use %newobject flag
        matrix<lfloat, row_major> rowMajorPeer(*this);
        *mat = new float[Rows() * Columns()];
        std::copy(rowMajorPeer.data().begin(), rowMajorPeer.data().end(), *mat);
        *rows = Rows();
        *cols = Columns();
    }

    void
    DenseMatrix::CheckInvariants(int column) const
    {
        // make sure no used entries are outside of the bands
        int start, end;
        boost::tie(start, end) = UsedRowRange(column);
        assert(0 <= start && start <= end && end <= Rows());
        for (int i = 0; i < Rows(); i++)
        {
            if (!(start <= i && i < end))
            {
                assert ((*this)(i, column) == value_type());
            }
        }
    }
}
