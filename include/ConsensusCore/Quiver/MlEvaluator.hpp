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

#pragma once


#include <xmmintrin.h>
#include <pmmintrin.h>

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <climits>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <ConsensusCore/Quiver/detail/SseMath.hpp>
#include <ConsensusCore/Quiver/QuiverConfig.hpp>
#include <ConsensusCore/Features.hpp>
#include <ConsensusCore/Types.hpp>
#include <ConsensusCore/Utils.hpp>
#include <ConsensusCore/Read.hpp>

#ifndef SWIG
using std::min;
using std::max;
#endif  // SWIG

#define NEG_INF -FLT_MAX

namespace ConsensusCore
{    
    /// \brief An Evaluator that can compute move scores using a MlSequenceFeatures
    class MlEvaluator
    {
    public:
        typedef MlSequenceFeatures FeaturesType;
        typedef MlModelParams      ParamsType;
        typedef Read<FeaturesType> ReadType;

    public:
        MlEvaluator(const ReadType& read,
                    const std::string& tpl,
                    const MlModelParams& params,
                    bool pinStart = true,
                    bool pinEnd = true)
            : read_(read)
            , params_(params)
            , tpl_(tpl)
            , pinStart_(pinStart)
            , pinEnd_(pinEnd)
        {}

        ~MlEvaluator()
        {}

        std::string ReadName() const
        {
            return read_.Name;
        }

        std::string Basecalls() const
        {
            return Features().Sequence();
        }

        std::string Template() const
        {
            return tpl_;
        }

        void Template(std::string tpl)
        {
            tpl_ = tpl;
        }


        int ReadLength() const
        {
            return Features().Length();
        }

        int TemplateLength() const
        {
            return tpl_.length();
        }

        bool PinEnd() const
        {
            return pinEnd_;
        }

        bool PinStart() const
        {
            return pinStart_;
        }

        bool IsMatch(int i, int j) const
        {
            assert(0 <= i && i < ReadLength());
            assert (0 <= j && j < TemplateLength());
            return (Features()[i] == tpl_[j]);
        }

        // match or mismatch
        float Inc(int i, int j) const
        {
            assert(0 <= j && j < TemplateLength() &&
                   0 <= i && i < ReadLength() );

            if (i == 0 && j == 0)
            {
                return params_.EmitMatch;
            }

            return ( read_.Features.TransitionProbabilities[i].Match
                   + (IsMatch(i, j) ? params_.EmitMatch : params_.EmitSubstitution) );
        }

        // deletion
        float Del(int i, int j) const
        {
            assert(0 <= j && j < TemplateLength() &&
                   0 <= i && i <= ReadLength() );
            if ( (!PinStart() && i == 0) || (!PinEnd() && i == ReadLength()) )
            {
                return 0.0f;
            }
            else
            {
                return read_.Features.TransitionProbabilities[i].Deletion;
            }
        }

        // insertion
        float Extra(int i, int j) const
        {
            assert(0 <= j && j <= TemplateLength() &&
                   0 <= i && i < ReadLength() );
            return ( j + 1 < TemplateLength() && IsMatch(i, j + 1) )
                   ? read_.Features.TransitionProbabilities[i].Branch
                   : read_.Features.TransitionProbabilities[i].Stick;
        }

        float Merge(int i, int j) const
        {
            assert(0 <= j && j < TemplateLength() - 1 &&
                   0 <= i && i < ReadLength() );
            return -FLT_MAX;
        }

        //
        // SSE
        //

        __m128 Inc4(int i, int j) const
        {
            assert (0 <= i && i <= ReadLength() - 4);
            assert (0 <= j && j < TemplateLength());
            __m128 res = _mm_setr_ps(Inc(i + 0, j),
                                     Inc(i + 1, j),
                                     Inc(i + 2, j),
                                     Inc(i + 3, j));
            return res;
        }

        __m128 Del4(int i, int j) const
        {
            assert (0 <= i && i <= ReadLength() - 4);
            assert (0 <= j && j < TemplateLength());
            // Have to do PinStart/PinEnd logic, and weird
            // logic for last row.  Punt.
            __m128 res = _mm_setr_ps(Del(i + 0, j),
                                     Del(i + 1, j),
                                     Del(i + 2, j),
                                     Del(i + 3, j));
            return res;
        }

        __m128 Extra4(int i, int j) const
        {
            assert (0 <= i && i <= ReadLength() - 4);
            assert (0 <= j && j <= TemplateLength());
            __m128 res = _mm_setr_ps(Extra(i + 0, j),
                                     Extra(i + 1, j),
                                     Extra(i + 2, j),
                                     Extra(i + 3, j));
            return res;
        }


        __m128 Merge4(int i, int j) const
        {
            return _mm_set_ps1(-FLT_MAX);
        }


        template<typename M>
        std::vector<MlTransitionProbabilities> PseudoCounts(const M& alpha, const M& beta) const
        {
            const int I = ReadLength();
            const int J = TemplateLength();
            std::vector<MlTransitionProbabilities> res(I);

            // TODO (lhepler) deal with sparse-matrix-fu
            for (int i = 0; i < I; ++i)
            {
                const MlTransitionProbabilities transProbs = read_.Features.TransitionProbabilities[i];

                for (int j = 0; j < J; ++j)
                {
                    double curF = alpha(i, j);

                    if (i + 1 < I && j + 1 < J)
                    {
                        bool isMatch = read_.Features[i + 1] == tpl_[i + 1];
                        double e = isMatch ? params_.EmitMatch : params_.EmitSubstitution;
                        res[i].Match += curF + beta(i + 1, j + 1) + transProbs.Match + e;
                    }

                    if (i + 1 < I)
                    {
                        bool isBranch = read_.Features[i] == read_.Features[i + 1];
                        if (isBranch)
                        {
                            res[i].Branch += curF + beta(i + 1, j) + transProbs.Branch;
                        }
                        else
                        {
                            res[i].Stick += curF + beta(i + 1, j) + transProbs.Stick;
                        }
                    }

                    if (j + 1 < J)
                    {
                        res[i].Deletion += curF + beta(i, j + 1) + transProbs.Deletion;
                    }
                }
            }

            return res;
        }
 

        template<typename M>
        std::pair<double, double> NewSubstitutionRate(const M& alpha, const M& beta) const
        {
            const int I = ReadLength();
            const int J = TemplateLength();
            const double ll = beta(0, 0);
            double num = -std::numeric_limits<double>::max();
            double den = -std::numeric_limits<double>::max();

            for (int i = 0; i < I; ++i)
            {
                for (int j = 0; j < J; ++j)
                {
                    double amt = ((i > 0 && j > 0) ? alpha(i - 1, j - 1) : 0.0) + Inc(i, j) + beta(i, j) - ll;
                    if (read_.Features[i] == tpl_[j])
                        num = lgAdd(num, amt);
                    den = lgAdd(den, amt);
                }
            }
            return std::make_pair(num, den);
        }


    protected:
        inline const MlSequenceFeatures& Features() const
        {
            return read_.Features;
        }


    protected:
        ReadType read_;
        MlModelParams params_;
        std::string tpl_;
        bool pinStart_;
        bool pinEnd_;
    };
}
