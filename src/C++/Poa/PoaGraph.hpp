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

#include <boost/tuple/tuple.hpp>
#include <vector>
#include <string>
#include <utility>

#include "Types.hpp"
#include "Mutation.hpp"

namespace ConsensusCore
{
    /// \brief An object representing a Poa (partial-order alignment) graph
    class PoaGraph
    {
        class Impl;
        Impl* impl;

    public:  // Flags enums for specifying GraphViz output features
        enum {
            COLOR_NODES    = 0x1,
            VERBOSE_NODES = 0x2
        };

    public:
        void AddSequence(const std::string& sequence, const PoaConfig& config);

        // TODO(dalexander): move this method to PoaConsensus so we don't have to use a tuple
        // interface here (which was done to avoid a circular dep on PoaConsensus).
#ifndef SWIG
        boost::tuple<std::string, float, std::vector< std::pair<Mutation*, float> >* >
        FindConsensus(const PoaConfig& config) const;
#endif  // !SWIG

        int NumSequences() const;
        std::string ToGraphViz(int flags = 0) const;
        void WriteGraphVizFile(std::string filename, int flags = 0) const;
        PoaGraph();
        ~PoaGraph();
    };
}

