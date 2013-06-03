
from pbcore.io import FastaReader, FastaWriter

import ConsensusCore as cc
import numpy as np
import numpy.random as nr

def printAln(ga):
    COLUMNS = 60
    acc  = ga.Accuracy()
    print "Acc:", acc
    xc = ga.Transcript()\
           .replace("M", " ")\
           .replace("I", "+")\
           .replace("D", "-")
    val = ""
    for i in xrange(0, len(xc), COLUMNS):
        val += "\n"
        val += "  " + ga.Target()  [i:i+COLUMNS] + "\n"
        val += "  " + xc  [i:i+COLUMNS] + "\n"
        val += "  " + ga.Query() [i:i+COLUMNS] + "\n"
        val += "\n"
    print val


def simSmrtBellReads(ccRng, seqParams, insert, readLength):
    fwdTpl = insert
    revTpl = cc.ReverseComplement(insert)

    reads = []
    while sum([len(r) for r in reads]) < readLength:
        if len(reads) % 2 == 0:
            tpl = fwdTpl
        else:
            tpl = revTpl
        simRead = cc.SimulateRead(seqParams, tpl, ccRng)

        ga = cc.Align(tpl, simRead)
        #printAln(ga)

        reads.append(simRead)

    # Truncate last read
    requiredLastReadLength = readLength - sum([len(r) for r in reads[-1]])
    reads[-1] = reads[-1][:requiredLastReadLength]
    return reads

def sampleInsertFromChromosome(chromosome, insertSize, circular=False):
    #
    # Returns tStart, tEnd, strand, insert
    #
    chromosomeLen = len(chromosome)
    strand = nr.randint(0, 2)
    start  = nr.randint(0, chromosomeLen)
    tStart = start
    tEnd   = ((tStart + insertSize) % chromosomeLen) if circular \
             else min(chromosomeLen, tStart + insertSize)
    insert_ = chromosome[tStart:tEnd] if tStart <= tEnd \
              else genome[:tEnd] + genome[tStart:]
    insert = cc.ReverseComplement(insert_) if strand \
             else insert_

    return tStart, tEnd, strand, insert


def simExperiment(seed, seqParams,
                  meanReadLength,
                  meanInsertSize,
                  genome,
                  numSmrtBells,
                  circular=True):
    nr.seed(seed)
    ccRng = cc.RandomNumberGenerator(seed)

    for moleculeId in xrange(numSmrtBells):
        chromosomeId = nr.randint(0, len(genome))
        chromosome = genome[chromosomeId]

        # No model for readLengths, insert lengths as yet
        readLength = meanReadLength
        insertSize = meanInsertSize

        tStart, tEnd, strand, insert = sampleInsertFromChromosome(chromosome,
                                                                  insertSize,
                                                                  circular)
        if len(insert) < 500: continue
        smrtBellReads = simSmrtBellReads(ccRng, seqParams, insert, readLength)

        for subreadId, subread in enumerate(smrtBellReads):
            yield moleculeId, subreadId, (strand + subreadId) % 2, \
                chromosomeId, tStart, tEnd, subread


if __name__ == '__main__':
    fw     = FastaWriter("/tmp/reads.fa")
    csvOut = open("/tmp/reads.csv", "w")
    csvOut.write("MoleculeId,SubReadId,Chromosome,tStart,tEnd,Strand\n")

    genome = [r.sequence for r in FastaReader("~/Data/lambdaNEB.fa")]
    #genome = [r.sequence for r in FastaReader("~/Data/Diploid/diploidLambda.fa")]
    seqParams = cc.SequencingParameters.C2()
    for readId, item in enumerate(simExperiment(42, seqParams,
                                                4000, 1000, genome, 50000, False)):
        moleculeId, subreadId, strand, chromosomeId, tStart, tEnd, subread = item

        rStart = subreadId
        rEnd   = subreadId

        readName = "mSimulator/%d" % moleculeId

        fw.writeRecord(readName, subread)

        csvOut.write("%d,%d,%d,%d,%d,%d" %
                     (moleculeId, subreadId, chromosomeId, tStart, tEnd, strand))
        csvOut.write("\n")
