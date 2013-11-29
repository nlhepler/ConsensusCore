using System;
using ConsensusCore;

namespace ConsensusCoreDemo
{
    /// <summary>
    /// Excercise the Edna use case of ConsensusCore -- doesn't do anything useful -- just
    /// makes sure the ConsensusCore/SWIG exports the interfaces required by Edna
    /// </summary>
    public class TestEdna
    {
        public IntArray ChannelArray()
        {
            var channel = new IntArray(8);

            var ch = new[] { 1, 2, 3, 4, 1, 2, 3, 4 };
            for (int i = 0; i < ch.Length; i++)
                channel.setitem(i, ch[i]);

            return channel;
        }

        public SWIGTYPE_p_float FloatArray(int n, float val)
        {
            var arr = new FloatArray(n);

            for (int i = 0; i < n; i++)
                arr.setitem(i, val);

            return arr.cast();
        }


        // public EdnaModelParams ModelParams()
        // {
        //     var modelParams = new EdnaModelParams(
        //         FloatArray(4, 0.1f),
        //         FloatArray(4, 0.1f),
        //         FloatArray(20, 0.2f),
        //         FloatArray(20, 0.2f));

        //     return modelParams;
        // }


        // public ChannelSequenceFeatures EdnaFeatures()
        // {
        //     var feature = new ChannelSequenceFeatures("ACGTACGT", ChannelArray().cast());
        //     return feature;
        // }

        // public void EdnaScorer()
        // {
        //     var eval = new EdnaEvaluator(EdnaFeatures(), "ACGTACGT", ChannelArray().cast(), ModelParams());
        //     var recursor = new SparseSseEdnaRecursor((int)Move.ALL_MOVES, new BandingOptions(0, 10));
        //     var scorer = new SparseSseEdnaMutationScorer(eval, recursor);

        //     scorer.ScoreMutation(MutationType.INSERTION, 3, 'A');

        //     var counter = new EdnaCounts();
        //     var intFeature = new IntFeature(10);
        //     var resultArray = new FloatArray(5);

        //     counter.DoCount(intFeature, eval, scorer, 3, 4, resultArray.cast());
        // }
    }


    public class Demo
    {

        public static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");

            var dm = new DenseMatrix(10, 10);
            Console.WriteLine(dm.Get(5, 5));

            Console.WriteLine("Score test:");
            Test1();

            Console.WriteLine("POA test");
            Test2();
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

        public static void Test1()
        {
            var bandOptions = new BandingOptions(4, 50);
            var recursor = new SimpleQvRecursor((int)Move.ALL_MOVES, bandOptions);

            var strandTpl = "ACGTACGTACGTACGT";

            QvModelParams modelParams = ModelParams();
            QvSequenceFeatures features = new QvSequenceFeatures("ACGTACGTCGT");

            var evaluator = new QvEvaluator(features, strandTpl, modelParams);

            var mutationEvaluator = new SimpleQvMutationScorer(evaluator, recursor);
            var score = mutationEvaluator.Score();
            Console.WriteLine(score);
        }

        public static void Test2()
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
            Console.WriteLine(r);
        }
    }
}
