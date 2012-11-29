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
#include <boost/assign.hpp>
#include <string>
#include <vector>

#include "Mutation.hpp"
#include "Sequence.hpp"
#include "Quiver/MutationScorer.hpp"
#include "Quiver/MultiReadMutationScorer.hpp"
#include "Quiver/PBFeatures.hpp"
#include "Quiver/QuiverConfig.hpp"
#include "Quiver/QvEvaluator.hpp"
#include "Quiver/SimpleRecursor.hpp"
#include "Quiver/SseRecursor.hpp"

#include "ParameterSettings.hpp"

using namespace ConsensusCore;  // NOLINT
using namespace boost::assign;  // NOLINT

typedef testing::Types<SimpleQvRecursor,
                       SseQvRecursor,
                       SparseSimpleQvRecursor,
                       SparseSseQvRecursor>    AllRecursorTypes;
TYPED_TEST_CASE(MutationScorerTest,          AllRecursorTypes);

TYPED_TEST_CASE(MultiReadMutationScorerTest, testing::Types<SparseSseQvRecursor>);

//
// ================== Tests for single read MutationScorer ============================
//

template <typename R>
class MutationScorerTest : public testing::Test
{
public:
    typedef MutationScorer<R> MS;

protected:
    MutationScorerTest()
        : recursor_(ALL_MOVES, BandingOptions(4, 200)),
          testingParams_(TestingParams<typename MS::EvaluatorType::ParamsType>())
    {}

    virtual ~MutationScorerTest() {}

protected:
    typename MS::RecursorType recursor_;
    typename MS::EvaluatorType::ParamsType testingParams_;
};


#define MS MutationScorer<TypeParam>
#define E  typename TypeParam::EvaluatorType

TYPED_TEST(MutationScorerTest, BasicTest)
{
    std::string tpl = "GATTACA";
    QvSequenceFeatures read("GATTACA");
    E ev(read, tpl, this->testingParams_, true, true);
    MS ms(ev, this->recursor_);
    Mutation insertMutation(INSERTION, 4, 'A');
    Mutation substitutionMutation(SUBSTITUTION, 4, 'T');
    Mutation deletionMutation(DELETION, 4, '-');

    // Testing mutations should not change the template.
    // Let's just make sure of that.
    EXPECT_EQ("GATTACA", ms.Template());
    EXPECT_EQ(0, ms.Score());
    EXPECT_EQ("GATTACA", ms.Template());
    EXPECT_EQ(-2, ms.ScoreMutation(insertMutation));
    EXPECT_EQ("GATTACA", ms.Template());
    EXPECT_EQ(-10, ms.ScoreMutation(substitutionMutation));
    EXPECT_EQ("GATTACA", ms.Template());
    EXPECT_EQ(-8, ms.ScoreMutation(deletionMutation));
    EXPECT_EQ("GATTACA", ms.Template());
}

TYPED_TEST(MutationScorerTest, TemplateMutationWorkflow)
{
    std::string tpl = "GATTACA";
    QvSequenceFeatures read("GATTACA");
    E ev(read, tpl, this->testingParams_, true, true);
    MS ms(ev, this->recursor_);
    Mutation insertMutation(INSERTION, 4, 'A');

    EXPECT_EQ("GATTACA", ms.Template());
    EXPECT_EQ(0, ms.Score());
    EXPECT_EQ("GATTACA", ms.Template());
    EXPECT_EQ(-2, ms.ScoreMutation(insertMutation));

    std::string newTpl = ApplyMutation(insertMutation, tpl);
    ms.Template(newTpl);
    EXPECT_EQ(-2, ms.Score());
    EXPECT_EQ("GATTAACA", ms.Template());
}


//
// ================== Tests for MultiReadMutationScorer ===========================
//

template <typename R>
class MultiReadMutationScorerTest : public testing::Test
{
public:
    typedef MultiReadMutationScorer<R> MMS;

protected:
    MultiReadMutationScorerTest()
        : testingConfig_(TestingParams<typename MMS::EvaluatorType::ParamsType>(),
                         ALL_MOVES,
                         BandingOptions(4, 200),
                         -500)
    {}

    virtual ~MultiReadMutationScorerTest() {}

protected:
    QuiverConfig testingConfig_;
};


#define MMS MultiReadMutationScorer<TypeParam>
#define E   typename TypeParam::EvaluatorType

TYPED_TEST(MultiReadMutationScorerTest, Template)
{
    //                 0123456789
    std::string fwd = "AAAATTTTGG";
    std::string rev = ReverseComplement(fwd);

    // Make sure the Template function works right
    MMS mScorer(this->testingConfig_, fwd);
    ASSERT_EQ(fwd, mScorer.Template());
    ASSERT_EQ(fwd, mScorer.Template(FORWARD_STRAND));
    ASSERT_EQ(rev, mScorer.Template(REVERSE_STRAND));
    ASSERT_EQ(fwd, mScorer.Template(FORWARD_STRAND, 0, 10));
    ASSERT_EQ(rev, mScorer.Template(REVERSE_STRAND, 0, 10));
    ASSERT_EQ("AT", mScorer.Template(FORWARD_STRAND, 3, 5));
    ASSERT_EQ("AT", mScorer.Template(REVERSE_STRAND, 3, 5));
    ASSERT_EQ("TTTT", mScorer.Template(FORWARD_STRAND, 4, 8));
    ASSERT_EQ("AAAA", mScorer.Template(REVERSE_STRAND, 4, 8));
}

TYPED_TEST(MultiReadMutationScorerTest, BasicTest)
{
    std::string tpl = "TTGATTACATT";
    std::string revTpl = ReverseComplement(tpl);

    MMS mScorer(this->testingConfig_, tpl);
    mScorer.AddRead(QvSequenceFeatures("TTGATTACATT"), FORWARD_STRAND);

    Mutation noOpMutation(SUBSTITUTION, 6, 'A');
    Mutation insertMutation(INSERTION, 6, 'A');
    Mutation substitutionMutation(SUBSTITUTION, 6, 'T');
    Mutation deletionMutation(DELETION, 6, '-');

    EXPECT_EQ(0, mScorer.Score(noOpMutation));
    EXPECT_EQ("TTGATTACATT", mScorer.Template());
    EXPECT_EQ(-2, mScorer.Score(insertMutation));
    EXPECT_EQ("TTGATTACATT", mScorer.Template());
    EXPECT_EQ(-10, mScorer.Score(substitutionMutation));
    EXPECT_EQ("TTGATTACATT", mScorer.Template());
    EXPECT_EQ(-8, mScorer.Score(deletionMutation));
    EXPECT_EQ("TTGATTACATT", mScorer.Template());

    mScorer.AddRead(QvSequenceFeatures("TTGATTACATT"), FORWARD_STRAND);

    EXPECT_EQ(0, mScorer.Score(noOpMutation));
    EXPECT_EQ("TTGATTACATT", mScorer.Template());
    EXPECT_EQ(-4, mScorer.Score(insertMutation));
    EXPECT_EQ("TTGATTACATT", mScorer.Template());
    EXPECT_EQ(-20, mScorer.Score(substitutionMutation));
    EXPECT_EQ("TTGATTACATT", mScorer.Template());
    EXPECT_EQ(-16, mScorer.Score(deletionMutation));
    EXPECT_EQ("TTGATTACATT", mScorer.Template());

    std::vector<Mutation*> muts;
    muts += &insertMutation;
    mScorer.ApplyMutations(muts);
    EXPECT_EQ("TTGATTAACATT", mScorer.Template());

    Mutation newNoOpMutation(SUBSTITUTION, 6, 'A');
    EXPECT_EQ(0, mScorer.Score(newNoOpMutation));
}


TYPED_TEST(MultiReadMutationScorerTest, ReverseStrandTest)
{
    // Just make sure if we reverse complemented the universe,
    // everything would come out the same.
    std::string tpl = "AATGTAATCAA";
    MMS mScorer(this->testingConfig_, tpl);
    mScorer.AddRead(QvSequenceFeatures("TTGATTACATT"), REVERSE_STRAND);

    Mutation noOpMutation(SUBSTITUTION, 4, 'T');
    Mutation insertMutation(INSERTION, 5, 'T');
    Mutation substitutionMutation(SUBSTITUTION, 4, 'A');
    Mutation deletionMutation(DELETION, 4, '-');

    EXPECT_EQ(0, mScorer.Score(noOpMutation));
    EXPECT_EQ("AATGTAATCAA", mScorer.Template());
    EXPECT_EQ(-2, mScorer.Score(insertMutation));
    EXPECT_EQ("AATGTAATCAA", mScorer.Template());
    EXPECT_EQ(-10, mScorer.Score(substitutionMutation));
    EXPECT_EQ("AATGTAATCAA", mScorer.Template());
    EXPECT_EQ(-8, mScorer.Score(deletionMutation));
    EXPECT_EQ("AATGTAATCAA", mScorer.Template());

    mScorer.AddRead(QvSequenceFeatures("TTGATTACATT"), REVERSE_STRAND);

    EXPECT_EQ(0, mScorer.Score(noOpMutation));
    EXPECT_EQ("AATGTAATCAA", mScorer.Template());
    EXPECT_EQ(-4, mScorer.Score(insertMutation));
    EXPECT_EQ("AATGTAATCAA", mScorer.Template());
    EXPECT_EQ(-20, mScorer.Score(substitutionMutation));
    EXPECT_EQ("AATGTAATCAA", mScorer.Template());
    EXPECT_EQ(-16, mScorer.Score(deletionMutation));
    EXPECT_EQ("AATGTAATCAA", mScorer.Template());

    std::vector<Mutation*> muts;
    muts += &insertMutation;
    mScorer.ApplyMutations(muts);
    EXPECT_EQ("AATGTTAATCAA", mScorer.Template());

    Mutation newNoOpMutation(SUBSTITUTION, 4, 'T');
    EXPECT_EQ(0, mScorer.Score(newNoOpMutation));
}


TYPED_TEST(MultiReadMutationScorerTest, NonSpanningReadsTest1)
{
    // read1:                     >>>>>>>>>>>
    // read2:          <<<<<<<<<<<
    //                 0123456789012345678901
    std::string tpl = "AATGTAATCAATTGATTACATT";
    MMS mScorer(this->testingConfig_, tpl);

    // mutations in the latter half
    Mutation noOpMutation1(SUBSTITUTION, 17, 'A');
    Mutation insertMutation1(INSERTION, 17, 'A');
    Mutation substitutionMutation1(SUBSTITUTION, 17, 'T');
    Mutation deletionMutation1(DELETION, 17, '-');

    // mutations in the first half
    Mutation noOpMutation2(SUBSTITUTION, 4, 'T');
    Mutation insertMutation2(INSERTION, 5, 'T');
    Mutation substitutionMutation2(SUBSTITUTION, 4, 'A');
    Mutation deletionMutation2(DELETION, 4, '-');

    mScorer.AddRead(QvSequenceFeatures("TTGATTACATT"), FORWARD_STRAND, 11, 22);
    mScorer.AddRead(QvSequenceFeatures("TTGATTACATT"), REVERSE_STRAND,  0, 11);

    EXPECT_EQ(0, mScorer.Score(noOpMutation1));
    EXPECT_EQ(-2, mScorer.Score(insertMutation1));
    EXPECT_EQ(-10, mScorer.Score(substitutionMutation1));
    EXPECT_EQ(-8, mScorer.Score(deletionMutation1));

    EXPECT_EQ(0, mScorer.Score(noOpMutation2));
    EXPECT_EQ(-2, mScorer.Score(insertMutation2));
    EXPECT_EQ(-10, mScorer.Score(substitutionMutation2));
    EXPECT_EQ(-8, mScorer.Score(deletionMutation2));

    EXPECT_EQ(tpl, mScorer.Template());

    std::vector<Mutation*> muts;
    muts += &insertMutation1, &insertMutation2;
    mScorer.ApplyMutations(muts);
    EXPECT_EQ("AATGTTAATCAATTGATTAACATT", mScorer.Template());
}
