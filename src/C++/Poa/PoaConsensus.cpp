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

#include "Poa/PoaConsensus.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <utility>
#include <vector>

#include "Poa/PoaConfig.hpp"
#include "Utils.hpp"

using boost::tie;

namespace ConsensusCore
{
    PoaConsensus::PoaConsensus(const PoaConfig& config)
        : config_(config),
          variants_(NULL)
    {
        poaGraph_ = new PoaGraph();
    }

    PoaConsensus::~PoaConsensus()
    {
        delete poaGraph_;
        if (variants_ != NULL)
        {
            delete variants_;
        }
    }

    const PoaConsensus*
    PoaConsensus::FindConsensus(const std::vector<std::string>& reads, const PoaConfig& config)
    {
        // do we need to filter zero-length reads here?
        PoaConsensus* pc = new PoaConsensus(config);
        foreach (const std::string& read, reads)
        {
            if (read.length() == 0)
            {
                throw InvalidInputError("Input sequences must have nonzero length.");
            }
            pc->poaGraph_->AddSequence(read, config);
        }
        tie(pc->consensusSequence_, pc->score_, pc->variants_) =
            pc->poaGraph_->FindConsensus(config);
        return pc;
    }

    const PoaConsensus*
    PoaConsensus::FindConsensus(const std::vector<std::string>& reads, bool global)
    {
        return FindConsensus(reads, PoaConfig(global));
    }

    const PoaConsensus*
    PoaConsensus::FindConsensus(const std::vector<std::string>& reads)
    {
        return PoaConsensus::FindConsensus(reads, PoaConfig());
    }

    const PoaGraph*
    PoaConsensus::Graph() const
    {
        return poaGraph_;
    }

    float
    PoaConsensus::Score() const
    {
        return score_;
    }

    std::string
    PoaConsensus::Sequence() const
    {
        return consensusSequence_;
    }

    std::string
    PoaConsensus::ToString() const
    {
        return "Poa Consensus: " + Sequence();
    }

    const std::vector< std::pair<Mutation*, float> >*
    PoaConsensus::Mutations() const
    {
        return variants_;
    }
}

