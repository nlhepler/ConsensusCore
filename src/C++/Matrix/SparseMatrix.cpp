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

#include "Matrix/SparseMatrix.hpp"

#include <algorithm>
#include <boost/tuple/tuple.hpp>

namespace ConsensusCore {
    // Performance insensitive routines are not inlined

    SparseMatrix::SparseMatrix(int rows, int cols)
        : columns_(cols),  nCols_(cols), nRows_(rows), columnBeingEdited_(-1),
          usedRanges_(cols, std::make_pair(0, 0))
    {
        for (int j = 0; j < nCols_; j++)
        {
            columns_[j] = NULL;
        }
    }

    SparseMatrix::~SparseMatrix()
    {
        for (int j = 0; j < nCols_; j++)
        {
            if (columns_[j] != NULL) delete columns_[j];
        }
    }

    int
    SparseMatrix::UsedEntries() const
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
    SparseMatrix::AllocatedEntries() const
    {
        int sum = 0;
        for (int j = 0; j < nCols_; j++)
        {
            sum += (columns_[j] != NULL ?
                    columns_[j]->AllocatedEntries() : 0);
        }
        return sum;
    }

    void
    SparseMatrix::ToHostMatrix(float** mat, int* rows, int* cols) const
    {
        *mat = new float[Rows() * Columns()];
        *rows = Rows();
        *cols = Columns();
        for (int i = 0; i < Rows(); i++) {
            for (int j = 0; j < Columns(); j++) {
                (*mat)[i * Columns() + j] = Get(i, j);
            }
        }
    }

    void
    SparseMatrix::CheckInvariants(int column) const
    {
        for (int j = 0; j < nCols_; j++)
         {
             if (columns_[j] != NULL) columns_[j]->CheckInvariants();
         }
    }
}
