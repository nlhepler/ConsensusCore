using System;
using ConsensusCore;

namespace PacBio.ConsensusCoreManaged.CSharp
{
    class TestConsensusCore
    {
        public void Simple()
        {
            var reads = new StringVector();
            reads.Add("TTTACAGGATAGTCCAGT");
            reads.Add("ACAGGATACCCCGTCCAGT");
            reads.Add("ACAGGATAGTCCAGT");
            reads.Add("TTTACAGGATAGTCCAGTCCCC");
            reads.Add("TTTACAGGATTAGTCCAGT");
            reads.Add("TTTACAGGATTAGGTCCCAGT");
            reads.Add("TTTACAGGATAGTCCAGT");
            string r = PoaConsensus.FindConsensus(reads).Sequence();
            //Assert.AreEqual("TTTACAGGATAGTCCAGT", r);
        }

        public static QvModelParams ModelParams()
        {
            return new QvModelParams(-1.41882f,
                                     -6.58979f,
                                     -0.366356f,
                                     -1.22393f,
                                     -0.30647f,
                                     -3.26889f,
                                     -0.899265f,
                                      0.0403404f,
                                     -0.377961f,
                                     -0.328803f,
                                     -2.65419f,
                                     -0.28016f);
        }

        public void Recursion()
        {
            var bandOptions = new BandingOptions(4, 50);
            var recursor = new SimpleQvRecursor((int) (Move.BASIC_MOVES | Move.MERGE), bandOptions);

            var strandTpl = "ACGTACGTACGTACGT";

            QvModelParams modelParams = ModelParams();
            var features = new QvSequenceFeatures("ACGTACGTCGT");

            var evaluator = new QvEvaluator(features, strandTpl, modelParams);

            var mutationEvaluator = new SimpleQvMutationScorer(evaluator, recursor);
            var score = mutationEvaluator.Score();
            Console.WriteLine(score);
        }
    }
}
