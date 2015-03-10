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

#include <ConsensusCore/Quiver/QuiverConfig.hpp>

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#define FALLBACK "*"

namespace ConsensusCore {

    namespace
    {
        double lgOneThird = -1.0986122886681098;
    }

    MlModelParams::MlModelParams(double substitutionRate)
        : EmitMatch(log(1.0 - substitutionRate))
        , EmitSubstitution(log(substitutionRate) + lgOneThird)
    {}

    const std::string MlModelParams::ChemistryName = FALLBACK;
    const std::string MlModelParams::ModelName     = "UnitEM";

    template<typename P>
    ModelConfig<P>::ModelConfig(const ParamsType& params,
                                int movesAvailable,
                                const BandingOptions& bandingOptions,
                                float fastScoreThreshold,
                                float addThreshold)
        : Params(params),
          MovesAvailable(movesAvailable),
          Banding(bandingOptions),
          FastScoreThreshold(fastScoreThreshold),
          AddThreshold(addThreshold)
    {}

    template<typename P>
    ModelConfig<P>::ModelConfig(const ModelConfig& qvConfig)
        : Params(qvConfig.Params),
          MovesAvailable(qvConfig.MovesAvailable),
          Banding(qvConfig.Banding),
          FastScoreThreshold(qvConfig.FastScoreThreshold),
          AddThreshold(qvConfig.AddThreshold)
    {}

    template struct ModelConfig<QvModelParams>;
    template struct ModelConfig<MlModelParams>;


    template<typename C>
    ModelConfigTable<C>::ModelConfigTable()
    {}

    template<typename C>
    bool ModelConfigTable<C>::InsertAs_(const std::string& name, const ConfigType& config)
    {
        const_iterator it;

        for (it = table.begin(); it != table.end(); it++)
            if (name.compare(it->first) == 0)
                return false;

        table.push_front(std::make_pair(name, config));

        return true;
    }

    template<typename C>
    bool ModelConfigTable<C>::InsertAs(const std::string& name, const ConfigType& config)
        throw(InvalidInputError)

    {
        if (name.compare(FALLBACK) == 0)
            throw InvalidInputError(
                "Cannot Insert(...) a ConfigType with chemistry '" FALLBACK "'");
        return InsertAs_(name, config);
    }

    template<typename C>
    bool ModelConfigTable<C>::Insert(const ConfigType& config)
        throw(InvalidInputError)
    {
        const std::string& name = config.Params.ChemistryName;
        return InsertAs(name, config);
    }

    template<typename C>
    bool ModelConfigTable<C>::InsertDefault(const ConfigType& config)
    {
        return InsertAs_(FALLBACK, config);
    }

    template<typename C>
    int ModelConfigTable<C>::Size() const
    {
        return table.size();
    }

    template<typename C>
    const typename ModelConfigTable<C>::ConfigType& ModelConfigTable<C>::At(const std::string& name) const
        throw(InvalidInputError)
    {
        const_iterator it;

        // If we find a direct match for the chemistry, use it
        for (it = table.begin(); it != table.end(); it++)
            if (it->first.compare(name) == 0)
                return it->second;

        // Fallback
        for (it = table.begin(); it != table.end(); it++)
            if (it->first.compare(FALLBACK) == 0)
                return it->second;

        throw InvalidInputError("Chemistry not found in ModelConfigTable");
    }

    template<typename C>
    std::vector<std::string> ModelConfigTable<C>::Keys() const
    {
        std::vector<std::string> keys;
        for (const_iterator it = table.begin(); it != table.end(); it++)
        {
            keys.push_back(it->first);
        }
        return keys;
    }

    template<typename C>
    typename ModelConfigTable<C>::const_iterator ModelConfigTable<C>::begin() const
    {
        return table.begin();
    }

    template<typename C>
    typename ModelConfigTable<C>::const_iterator ModelConfigTable<C>::end() const
    {
        return table.end();
    }

    template class ModelConfigTable<QuiverConfig>;
    template class ModelConfigTable<MlConfig>;
}
