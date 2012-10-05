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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "PairwiseAlignment.hpp"
#include "AffineAlignment.hpp"

using namespace ConsensusCore;  // NOLINT
using ::testing::ElementsAreArray;

TEST(PairwiseAlignmentTests, RepresentationTests)
{
    PairwiseAlignment a("GATC", "GA-C");
    EXPECT_EQ("GATC", a.Target());
    EXPECT_EQ("GA-C", a.Query());
    EXPECT_EQ(4, a.Length());
    EXPECT_EQ(3, a.Matches());
    EXPECT_EQ(1, a.Deletions());
    EXPECT_EQ(0, a.Mismatches());
    EXPECT_EQ(0, a.Insertions());
    EXPECT_FLOAT_EQ(0.75, a.Accuracy());
    EXPECT_EQ("MMDM", a.Transcript());

    PairwiseAlignment a2("GATTA-CA",
                         "CA-TAACA");
    EXPECT_EQ("RMDMMIMM", a2.Transcript());
    EXPECT_FLOAT_EQ(5./8, a2.Accuracy());
    EXPECT_EQ(1, a2.Mismatches());
    EXPECT_EQ(1, a2.Deletions());
    EXPECT_EQ(1, a2.Insertions());
    EXPECT_EQ(5, a2.Matches());
}


TEST(PairwiseAlignmentTests, GlobalAlignmentTests)
{
    PairwiseAlignment* a = Align("GATT", "GATT");
    EXPECT_FLOAT_EQ(1.0, a->Accuracy());
    EXPECT_EQ("GATT", a->Target());
    EXPECT_EQ("GATT", a->Query());
    EXPECT_EQ("MMMM", a->Transcript());
    delete a;

    a = Align("GATT", "GAT");
    EXPECT_FLOAT_EQ(0.75, a->Accuracy());
    EXPECT_EQ("GATT", a->Target());
    EXPECT_EQ("GA-T", a->Query());
    EXPECT_EQ("MMDM", a->Transcript());
    delete a;

    a = Align("GATTACA", "TT");
    EXPECT_EQ("GATTACA", a->Target());
    EXPECT_EQ("--TT---", a->Query());
    EXPECT_FLOAT_EQ(2./7, a->Accuracy());
    delete a;
}


TEST(PairwiseAlignmentTests, TargetPositionsInQueryTest)
{
    // MMM -> 0123
    {
        int expected[] = { 0, 1, 2, 3 };
        ASSERT_THAT(TargetToQueryPositions("MMM"), ElementsAreArray(expected));
    }

    // DMM -> 0012, MDM -> 0112, MMD -> 0122,
    {
        int expected1[] = { 0, 0, 1, 2 };
        int expected2[] = { 0, 1, 1, 2 };
        int expected3[] = { 0, 1, 2, 2 };
        ASSERT_THAT(TargetToQueryPositions("DMM"), ElementsAreArray(expected1));
        ASSERT_THAT(TargetToQueryPositions("MDM"), ElementsAreArray(expected2));
        ASSERT_THAT(TargetToQueryPositions("MMD"), ElementsAreArray(expected3));
    }

    // IMM -> 123, MIM -> 023, MMI -> 013,
    {
        int expected1[] = { 1, 2, 3 };
        int expected2[] = { 0, 2, 3 };
        int expected3[] = { 0, 1, 3 };
        ASSERT_THAT(TargetToQueryPositions("IMM"), ElementsAreArray(expected1));
        ASSERT_THAT(TargetToQueryPositions("MIM"), ElementsAreArray(expected2));
        ASSERT_THAT(TargetToQueryPositions("MMI"), ElementsAreArray(expected3));
    }

    // MRM, MDIM -> 0123
    // MIDM -> 0223
    {
        int expected1[] = { 0, 1, 2, 3 };
        int expected2[] = { 0, 2, 2, 3 };
        ASSERT_THAT(TargetToQueryPositions("MRM"),  ElementsAreArray(expected1));
        ASSERT_THAT(TargetToQueryPositions("MDIM"), ElementsAreArray(expected1));
        ASSERT_THAT(TargetToQueryPositions("MIDM"), ElementsAreArray(expected2));
    }
}


// ------------------ AffineAlignment tests ---------------------

TEST(AffineAlignmentTests, GlobalAlignmentTests)
{
    // Start by checking that when the "affine" penalty is really just
    // linear, we get the same results we would have gotten using the
    // ordinary aligner.
    AffineAlignmentParams nonAffineParams(0, -1, -1, -1);

    PairwiseAlignment* a = AlignWithAffineGapPenalty("ATT", "ATT", nonAffineParams);
    EXPECT_EQ("ATT", a->Target());
    EXPECT_EQ("ATT", a->Query());

    a = AlignWithAffineGapPenalty("AT", "ATT", nonAffineParams);
    EXPECT_EQ("A-T", a->Target());
    EXPECT_EQ("ATT", a->Query());

    a = AlignWithAffineGapPenalty("GA", "GAT", nonAffineParams);
    EXPECT_EQ("GA-", a->Target());
    EXPECT_EQ("GAT", a->Query());

    a = AlignWithAffineGapPenalty("GAT", "GA", nonAffineParams);
    EXPECT_EQ("GAT", a->Target());
    EXPECT_EQ("GA-", a->Query());

    a = AlignWithAffineGapPenalty("GA", "TGA", nonAffineParams);
    EXPECT_EQ("-GA", a->Target());
    EXPECT_EQ("TGA", a->Query());

    a = AlignWithAffineGapPenalty("TGA", "GA", nonAffineParams);
    EXPECT_EQ("TGA", a->Target());
    EXPECT_EQ("-GA", a->Query());

    a = AlignWithAffineGapPenalty("GATTACA", "GATTTACA", nonAffineParams);
    EXPECT_EQ("GA-TTACA", a->Target());
    EXPECT_EQ("GATTTACA", a->Query());
}


