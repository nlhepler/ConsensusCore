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

#include <ConsensusCore/Checksum.hpp>
#include <ConsensusCore/Features.hpp>
#include <ConsensusCore/Types.hpp>
#include <ConsensusCore/Read.hpp>

#include <string>
#include <sstream>
#include <boost/format.hpp>

namespace ConsensusCore
{
    template<typename F>
    Read<F>::Read(const FeaturesType& features,
                  const std::string& name,
                  const std::string& chemistry)
        : Name(name)
        , Chemistry(chemistry)
        , Features(features)
    {}

    template<typename F>
    Read<F>::Read(const Read& other)
        : Name(other.Name)
        , Chemistry(other.Chemistry)
        , Features(other.Features)
    {}

    template<typename F>
    int Read<F>::Length() const
    {
        return Features.Length();
    }

    template<typename F>
    std::string Read<F>::ToString() const
    {
        return (boost::format("%s (%s) Length=%d Data=%s")
                % Name % Chemistry % Length() % Checksum::Of(Features)).str();
    }

    template<typename F>
    Read<F> Read<F>::Null()
    {
        return Read(FeaturesType(""), "", "");
    }

    template struct Read<QvSequenceFeatures>;
    template struct Read<MlSequenceFeatures>;


    template<typename F>
    MappedRead<F>::MappedRead(const ReadType& read,
                              StrandEnum strand,
                              int templateStart,
                              int templateEnd,
                              bool pinStart,
                              bool pinEnd)
        : Read<F>(read)
        , Strand(strand)
        , TemplateStart(templateStart)
        , TemplateEnd(templateEnd)
        , PinStart(pinStart)
        , PinEnd(pinEnd)
    {}

    template<typename F>
    MappedRead<F>::MappedRead(const MappedRead<F>& other)
        : Read<F>(other)
        , Strand(other.Strand)
        , TemplateStart(other.TemplateStart)
        , TemplateEnd(other.TemplateEnd)
        , PinStart(other.PinStart)
        , PinEnd(other.PinEnd)
    {}

    template<typename F>
    std::string MappedRead<F>::ToString() const
    {
        std::stringstream ss;
        ss << (PinStart ? "[" : "(");
        ss << TemplateStart << "," << TemplateEnd;
        ss << (PinEnd   ? "]" : ")");
        return Read<F>::ToString() + " @ " + ss.str();
    }

    template struct MappedRead<QvSequenceFeatures>;
    template struct MappedRead<MlSequenceFeatures>;
}
