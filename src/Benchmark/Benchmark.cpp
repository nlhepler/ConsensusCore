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

#include <string>
#include <iostream>
#include <ctime>
#include <cstdio>

#include "Random.hpp"

#include "Mutation.hpp"
#include "Simulation/Random.hpp"
#include "Simulation/Simulator.hpp"
#include "Poa/PoaConsensus.hpp"
#include "Quiver/MultiReadMutationScorer.hpp"

using namespace ConsensusCore;  // NOLINT
using namespace std;            // NOLINT


QvModelParams TestingParams()
{
    return QvModelParams(0.f,    // Match
                         -10.f,  // Mismatch
                         -0.1f,  // MismatchS
                         -5.f,   // Branch
                         -0.1f,  // BranchS
                         -4.f,   // DeletionN
                         -6.f,   // DeletionWithTag
                         -0.1f,  // DeletionWithTagS
                         -8.f,   // Nce
                         -0.1f,  // NceS
                         -2.f,   // Merge
                         0.f);   // MergeS
}

QuiverConfig TestingConfig()
{
    return QuiverConfig(TestingParams(),
                        ALL_MOVES,
                        BandingOptions(4, 200),
                        -12.5);
}


vector<Mutation*> allSingleBaseMutations(string tpl)
{
    vector<Mutation*> muts;
    const char* otherBases[] = { "A", "C", "G", "T" };

    for (int i=0; i < tpl.length(); i++)
    {
        muts.push_back(new Mutation(DELETION, i, i+1, ""));
        foreach (const char* otherBase, otherBases)
        {
            muts.push_back(new Mutation(INSERTION, i, i, otherBase));
            muts.push_back(new Mutation(SUBSTITUTION, i, i+1, otherBase));
        }
    }
    return muts;
}


float getTime()
{
    return float(clock())/CLOCKS_PER_SEC;
}


float BenchmarkLoop(int tplLength, int readDepth, int nIters=20)
{



    //
    // Build random template and reads (no QVs)
    //
    cout << "Building simulated data" << endl;
    Rng rng(42);
    string tpl = RandomSequence(rng, tplLength);
    SequencingParameters c2 = SequencingParameters::C2();
    RandomNumberGenerator rng2(42);
    vector<string> reads;
    for (int i = 0; i < readDepth; i++)
    {
        reads.push_back(SimulateRead(c2, tpl, rng2));
    }

    // vector<string> someReads(reads.begin(), reads.begin() + 5);
    // string poa = PoaConsensus::FindConsensus(someReads)->Sequence();
    string poa = tpl;

    SparseSseQvMultiReadMutationScorer mms(TestingConfig(), poa);
    foreach (string& read, reads)
    {
        mms.AddRead(MappedRead(QvSequenceFeatures(read),
                               FORWARD_STRAND, 0, tplLength));
    }

    // Generate all the mutations we will try
    vector<Mutation*> muts = allSingleBaseMutations(poa);

    cout << "Beginning benchmark loop" << endl;
    float start = getTime();
    for (int iter = 0; iter < nIters; iter++)
    {
        cout << iter << endl;
        foreach (const Mutation* m, muts)
        {
            //        volatile bool isFavorable = mms.FastIsFavorable(*m);
            volatile bool isFavorable = mms.IsFavorable(*m);

        }
    }
    float stop  = getTime();
    float elapsed = stop - start;
    cout << "Done" << endl;
    return elapsed;
}


int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 4)
    {
        cout << "Usage: benchmark <tplLength> readDepth> [nIters]" << endl;
        exit(-1);
    }

    int tplLength, readDepth, nIters;
    tplLength = atoi(argv[1]);
    readDepth = atoi(argv[2]);
    nIters = (argc > 3 ? atoi(argv[3]) : 20);

    float elapsed = BenchmarkLoop(tplLength, readDepth, nIters);

    printf("Elapsed: %.2f\n", elapsed);

    return 0;
}
