

from timeit import *
from os import getpid
from ConsensusCore import *

# A simple testing setup

tpl = "GATTACA" * 200
read = "GATTACA" * 100 + "GATTTACA" * 50 + "GAGGACA" * 50

I = len(read)
J = len(tpl)

features = QvSequenceFeatures(read)
params = QvModelParams.Default()

e = QvEvaluator(features, tpl, params)

simple_recursor = SparseSimpleQvRecursor(BASIC_MOVES | MERGE, BandingOptions(4, 200))
sse_recursor    = SparseSseQvRecursor(BASIC_MOVES | MERGE, BandingOptions(4, 200))

null_guide = SparseMatrix.Null()
alpha = SparseMatrix(I + 1, J + 1)
beta = SparseMatrix(I + 1, J + 1) 
ext = SparseMatrix(I + 1, 2)


ITERS = 100

#
#  Fill Alpha
#
def fill_alpha(recursor):
    recursor.FillAlpha(e, null_guide, alpha)

simple_fill_alpha_t = Timer("fill_alpha(simple_recursor)", "from __main__ import fill_alpha, simple_recursor")
simple_fill_alpha_time_per = simple_fill_alpha_t.timeit(number = ITERS) / ITERS

sse_fill_alpha_t = Timer("fill_alpha(sse_recursor)", "from __main__ import fill_alpha, sse_recursor")
sse_fill_alpha_time_per = sse_fill_alpha_t.timeit(number = ITERS) / ITERS

#
#  Fill Beta
#
def fill_beta(recursor):
    recursor.FillBeta(e, null_guide, beta)

simple_fill_beta_t = Timer("fill_beta(simple_recursor)", "from __main__ import fill_beta, simple_recursor")
simple_fill_beta_time_per = simple_fill_beta_t.timeit(number = ITERS) / ITERS

sse_fill_beta_t = Timer("fill_beta(sse_recursor)", "from __main__ import fill_beta, sse_recursor")
sse_fill_beta_time_per = sse_fill_beta_t.timeit(number = ITERS) / ITERS

#
# Extend Alpha
#
def extend_alpha(recursor):
    for j in range(3, J - 3):
        recursor.ExtendAlpha(e, alpha, j, ext)

simple_extend_t = Timer("extend_alpha(simple_recursor)", "from __main__ import extend_alpha, simple_recursor")
simple_extend_time_per = simple_extend_t.timeit(number = ITERS) / ITERS

sse_extend_t = Timer("extend_alpha(sse_recursor)", "from __main__ import extend_alpha, sse_recursor")
sse_extend_time_per = sse_extend_t.timeit(number = ITERS) / ITERS

#
# Link Alpha, Beta
#
def link_alpha_beta(recursor):
    for j in range(3, J - 3):
        recursor.LinkAlphaBeta(e, alpha, j, beta, j, j)

sse_recursor.FillAlphaBeta(e, alpha, beta)
simple_link_t = Timer("link_alpha_beta(simple_recursor)", "from __main__ import link_alpha_beta, simple_recursor")
simple_link_time_per = simple_link_t.timeit(number = ITERS) / ITERS

sse_link_t = Timer("link_alpha_beta(sse_recursor)", "from __main__ import link_alpha_beta, sse_recursor")
sse_link_time_per = sse_link_t.timeit(number = ITERS) / ITERS
    
    
print "Summary: (problem size = %d x %d;  %d iterations)" % (I, J, ITERS)
print "  FillAlpha:"
print "    Simple: %f sec/iter" % simple_fill_alpha_time_per
print "    SSE:    %f sec/iter" % sse_fill_alpha_time_per
print "  FillBeta:"
print "    Simple: %f sec/iter" % simple_fill_beta_time_per
print "    SSE:    %f sec/iter" % sse_fill_beta_time_per
print "  ExtendAlpha:"
print "    Simple: %f sec/iter" % simple_extend_time_per
print "    SSE:    %f sec/iter" % sse_extend_time_per
print "  LinkAlphaBeta:"
print "    Simple: %f sec/iter" % simple_link_time_per
print "    SSE:    %f sec/iter" % sse_link_time_per

print
print "FillAlpha,Sparse,Simple,%f" % simple_fill_alpha_time_per
print "FillAlpha,Sparse,SSE,%f" % sse_fill_alpha_time_per
print "FillBeta,Sparse,Simple,%f" % simple_fill_beta_time_per
print "FillBeta,Sparse,SSE,%f" % sse_fill_beta_time_per
print "ExtendAlpha,Sparse,Simple,%f" % simple_extend_time_per
print "ExtendAlpha,Sparse,SSE,%f" % sse_extend_time_per
print "LinkAlphaBeta,Sparse,Simple,%f" % simple_link_time_per
print "LinkAlphaBeta,Sparse,SSE,%f" % sse_link_time_per
print
