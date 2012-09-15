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
#include <google/profiler.h>

#include <boost/algorithm/string.hpp>
#include <boost/assign.hpp>
#include <boost/assign/std/vector.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "Features.hpp"
#include "Poa/PoaConfig.hpp"
#include "Poa/PoaConsensus.hpp"
#include "Quiver/PBFeatures.hpp"
#include "Quiver/SseRecursor.hpp"


using std::string;
using std::vector;
using std::cout;
using std::endl;
using boost::erase_all_copy;

using namespace boost::assign;  // NOLINT
using namespace ConsensusCore;  // NOLINT

//
// This is a mockup of how Primary CCS works, providing a convenient platform for
// benchmarking/profiling.
//
TEST(PrimaryWorkflow, FullStack)
{
    // ProfilerStart("my-profile.pprof");



    // ProfilerStop();
}



