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


#include "Poa/PoaConfig.hpp"

namespace ConsensusCore
{
    PoaParameterSet PoaParameterSet::DefaultParameters()
    {
        PoaParameterSet p;
        p.Extra    = -4;
        p.Missing  = -4;
        p.Mismatch = -5;
        p.Match    =  3;
        p.Branch   = -2;
        return p;
    }

    void PoaConfig::init(PoaParameterSet params, bool useLocalAlignment, bool useMergeMove)
    {
        this->Params = params;
        this->UseLocalAlignment = useLocalAlignment;
        this->UseMergeMove = useMergeMove;
    }

    PoaConfig::PoaConfig(PoaParameterSet params, bool useLocalAlignment, bool useMergeMove)
    {
        init(params, useLocalAlignment, useMergeMove);
    }

    PoaConfig::PoaConfig(PoaParameterSet params, bool useLocalAlignment)
    {
        init(params, useLocalAlignment, false);
    }

    PoaConfig::PoaConfig(bool useLocalAlignment)
    {
        init(PoaParameterSet::DefaultParameters(), useLocalAlignment, false);
    }

    PoaConfig::PoaConfig()
    {
        init(PoaParameterSet::DefaultParameters(), false, false);
    }
}
