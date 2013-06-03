
import sys
from pbcore.io import FastaReader, FastaWriter
from StringIO import StringIO

BASES = "ACGT"

def mutatedBase(base):
    return BASES[(BASES.index(base) + 1) % 4]

if __name__ == '__main__':
    inFile = sys.argv[1]
    outFile = sys.argv[2]
    inputContig = next(iter(FastaReader(inFile))).sequence
    mutPositions = xrange(249, len(inputContig), 500)
    mutContigIO = StringIO()
    lastPos = 0
    for pos in mutPositions:
        mutContigIO.write(inputContig[lastPos:pos])
        mutContigIO.write(mutatedBase(inputContig[pos]))
        lastPos = pos + 1
    mutContigIO.write(inputContig[lastPos:])
    mutContig = mutContigIO.getvalue()

    fw = FastaWriter(outFile)
    fw.writeRecord("original", inputContig)
    fw.writeRecord("mutated", mutContig)
