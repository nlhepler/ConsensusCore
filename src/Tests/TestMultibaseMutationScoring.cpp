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

#include <gtest/gtest.h>
#include <boost/assign.hpp>
#include <string>

#include "Quiver/MutationScorer.hpp"
#include "Quiver/PBFeatures.hpp"
#include "Quiver/QuiverConfig.hpp"
#include "Quiver/QvEvaluator.hpp"
#include "Quiver/SimpleRecursor.hpp"
#include "Quiver/SseRecursor.hpp"
#include "Quiver/ReadScorer.hpp"

#include "ParameterSettings.hpp"
#include "MatrixPrinting.hpp"


using namespace ConsensusCore;  // NOLINT
using namespace boost::assign;  // NOLINT

TEST(MultibaseMutationScoring, DinucleotideInsertionTest)
{
    QuiverConfig config = TestingConfig<QuiverConfig>();

    //                     0123456789012345678
    std::string tplTT   = "CCCCCGATTACACCCCC";
    std::string tplTTTT = "CCCCCGATTTTACACCCCC";
    std::string tplGCTT = "CCCCCGAGCTTACACCCCC";
    std::string tplAATT = "CCCCCGAAATTACACCCCC";

    QvSequenceFeatures read("CCCCCGATTTTACACCCCC");
    ReadScorer ez(config);
    float scoreTTTT = ez.Score(tplTTTT, read);
    EXPECT_EQ(0, scoreTTTT);

    QvEvaluator e(read, tplTT, config.QvParams);
    SparseSimpleQvRecursor r(config.MovesAvailable, config.Banding);
    SparseSimpleQvMutationScorer ms(e, r);

    EXPECT_EQ(0, ms.ScoreMutation(INSERTION, 7, 7, "TT"));
    EXPECT_EQ(0, ms.ScoreMutation(INSERTION, 8, 8, "TT"));
    EXPECT_EQ(0, ms.ScoreMutation(INSERTION, 9, 9, "TT"));

    EXPECT_EQ(ez.Score(tplGCTT, read), ms.ScoreMutation(INSERTION, 7, 7, "GC"));
    EXPECT_EQ(ez.Score(tplAATT, read), ms.ScoreMutation(INSERTION, 7, 7, "AA"));
    EXPECT_EQ(ez.Score(tplAATT, read), ms.ScoreMutation(INSERTION, 6, 6, "AA"));
}

TEST(MultibaseMutationScoring, DinucleotideDeletionTest)
{
    QuiverConfig config = TestingConfig<QuiverConfig>();

    //                     0123456789012345678
    std::string tplTT   = "CCCCCGATTACACCCCC";
    std::string tplTTTT = "CCCCCGATTTTACACCCCC";
    std::string tplGCTT = "CCCCCGAGCTTACACCCCC";

    QvSequenceFeatures read("CCCCCGATTACACCCCC");
    ReadScorer ez(config);
    float scoreTT = ez.Score(tplTT, read);
    EXPECT_EQ(0, scoreTT);

    QvEvaluator e(read, tplTTTT, config.QvParams);
    SparseSimpleQvRecursor r(config.MovesAvailable, config.Banding);
    SparseSimpleQvMutationScorer ms(e, r);

    EXPECT_EQ(scoreTT, ms.ScoreMutation(DELETION, 7, 9, ""));
    EXPECT_EQ(scoreTT, ms.ScoreMutation(DELETION, 8, 10, ""));
    EXPECT_EQ(scoreTT, ms.ScoreMutation(DELETION, 9, 11, ""));

    QvEvaluator e2(read, tplGCTT, config.QvParams);
    SparseSimpleQvRecursor r2(config.MovesAvailable, config.Banding);
    SparseSimpleQvMutationScorer ms2(e2, r2);
    EXPECT_EQ(scoreTT, ms2.ScoreMutation(DELETION, 7, 9, ""));
}
