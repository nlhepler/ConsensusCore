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

#include <gtest/gtest.h>

#include <boost/format.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "Matrix/SparseVector.hpp"

using namespace ConsensusCore; // NOLINT

TEST(SparseVectorTest, BasicTest)
{
    SparseVector sv(100, 10, 20);
    EXPECT_LE(10, sv.AllocatedEntries());

    for (int i = 0; i < 100; i++)
    {
        EXPECT_EQ(sv(i), -FLT_MAX);
    }

    for (int i = 10; i < 20; i++)
    {
        sv.Set(i, i);
    }
    for (int i = 0; i < 100; i++)
    {
        if (i >= 10 && i < 20) EXPECT_EQ(i, sv(i));
        else EXPECT_EQ(-FLT_MAX, sv(i)); // NOLINT
    }

    sv.Set(50, 50);
    EXPECT_LE(40, sv.AllocatedEntries());
    for (int i = 0; i < 100; i++)
    {
        if (i >= 10 && i < 20) EXPECT_EQ(i, sv(i));
        else if (i == 50) EXPECT_EQ(i, sv(i));
        else EXPECT_EQ(-FLT_MAX, sv(i)); // NOLINT
    }
}


TEST(SparseVectorTest, BasicTest2)
{
    SparseVector sv(100, 50, 60);

    sv.Set(5, 5);
    for (int i = 0; i < 100; i++)
    {
        if (i == 5) EXPECT_EQ(i, sv(i));
        else EXPECT_EQ(-FLT_MAX, sv(i)); // NOLINT
    }
}

