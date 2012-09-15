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

namespace ConsensusCore
{
    struct PoaParameterSet
    {
        float Extra;
        float Missing;
        float Mismatch;
        float Match;
        float Branch;

        static PoaParameterSet DefaultParameters();
    };

    struct PoaConfig
    {
        enum
        {
            GLOBAL_ALIGNMENT = false,
            LOCAL_ALIGNMENT = true
        };
        enum
        {
            USE_MERGE_MOVE = true,
            NO_MERGE_MOVE = false
        };

        PoaParameterSet Params;
        bool UseMergeMove;
        bool UseLocalAlignment;

        PoaConfig(PoaParameterSet params, bool useLocalAlignment, bool useMergeMove);
        PoaConfig(PoaParameterSet params, bool useLocalAlignment);
        explicit PoaConfig(bool useLocalAlignment);
        PoaConfig();

    private:
        void init(PoaParameterSet params, bool useLocalAlignment, bool useMergeMove);
    };
}
