# Print out a BED file of k-covered intervals for data in a cmp.h5
#
# % coverage.py filename [k]
#
# (k defaults to 5)
#

import numpy as np
from pbcore.io import (CmpH5Reader,
                       rangeQueries as RQ)

from ConsensusCore import CoveredIntervals

#c = CmpH5Reader("~/Data/fluidigm_amplicons/57147.cmp.h5")
#c = CmpH5Reader("~/Data/fluidigm_amplicons/56640-hg19.cmp.h5")

import sys
c = CmpH5Reader(sys.argv[1])
if len(sys.argv) >= 3:
    k = int(sys.argv[2])
else:
    k = 5

assert c.isSorted

def kCoveredIntervals(k, tStart, tEnd, winStart=None, winEnd=None):
    winStart = winStart or np.min(tStart)
    winEnd   = winEnd   or np.max(tStart)
    return CoveredIntervals(k, tStart, tEnd, int(winStart), int(winEnd-winStart))

def intervalToBed(name, interval):
    s, e = interval
    return "%s\t%d\t%d" % (name, s, e)


for refName in c.referenceInfoTable.FullName:
    startRow = c.referenceInfo(refName).StartRow
    endRow   = c.referenceInfo(refName).EndRow
    #print startRow, endRow

    goodMapQVs = (c.MapQV[startRow:endRow] >= 10)
    intervals = kCoveredIntervals(k,
                                  c.tStart[startRow:endRow][goodMapQVs],
                                  c.tEnd[startRow:endRow][goodMapQVs])
    for iv in intervals:
        print intervalToBed(refName, iv)
