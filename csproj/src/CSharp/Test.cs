using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using ConsensusCore;

namespace PacBio.Analysis.ConsensusCoreManaged.src.CSharp
{
    [TestFixture]
    class TestConsensusCore
    {
        [Test]
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
            Assert.AreEqual("TTTACAGGATAGTCCAGT", r);
        }

        [Test]
        public void Recursion()
        {
            var bandOptions = new BandingOptions(4, 50);
            var recursor = new SimpleQvRecursor((int) (Move.BASIC_MOVES | Move.MERGE), bandOptions);

            var strandTpl = "ACGTACGTACGTACGT";

            QvModelParams modelParams = QvModelParams.Default();
            QvSequenceFeatures features = new QvSequenceFeatures("ACGTACGTCGT");

            var evaluator = new QvEvaluator(features, strandTpl, modelParams);

            var mutationEvaluator = new SimpleQvMutationScorer(evaluator, recursor);
            var score = mutationEvaluator.Score();
            Console.WriteLine(score);
        }
    }
}
