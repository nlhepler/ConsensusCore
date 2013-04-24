%{
/* Includes the header in the wrapper code */
#include "Mutation.hpp"
#include "Quiver/MappedRead.hpp"
#include "Quiver/MultiReadMutationScorer.hpp"
#include "Quiver/MutationScorer.hpp"
#include "Quiver/QuiverConfig.hpp"    
#include "Quiver/SimpleRecursor.hpp"
#include "Quiver/SseRecursor.hpp"
#include "Quiver/ReadScorer.hpp"
#include "Sequence.hpp"
using namespace ConsensusCore;
%}

%include "Sequence.hpp"
%include "Mutation.hpp"
%include "Quiver/MappedRead.hpp"
%include "Quiver/detail/Combiner.hpp"
%include "Quiver/detail/RecursorBase.hpp"
%include "Quiver/MultiReadMutationScorer.hpp"
%include "Quiver/MutationScorer.hpp"
%include "Quiver/QuiverConfig.hpp"
%include "Quiver/SimpleRecursor.hpp"
%include "Quiver/SseRecursor.hpp"
%include "Quiver/ReadScorer.hpp"

namespace ConsensusCore {
    //
    // Dense matrix recursors and such
    //
    %template(QvRecursorBase)           detail::RecursorBase<DenseMatrix, QvEvaluator, detail::ViterbiCombiner>;
    %template(SimpleQvRecursor)         SimpleRecursor<DenseMatrix, QvEvaluator, detail::ViterbiCombiner>;
    %template(SimpleQvMutationScorer)   MutationScorer<SimpleQvRecursor>;
    %template(SseQvRecursor)            SseRecursor<DenseMatrix, QvEvaluator, detail::ViterbiCombiner>;
    %template(SseQvMutationScorer)      MutationScorer<SseQvRecursor>;
	
    //
    // Sparse matrix support
    //
    %template(SparseQvRecursorBase)           detail::RecursorBase<SparseMatrix, QvEvaluator, detail::ViterbiCombiner>;
    %template(SparseSimpleQvRecursor)         SimpleRecursor<SparseMatrix, QvEvaluator, detail::ViterbiCombiner>;
    %template(SparseSimpleQvMutationScorer)   MutationScorer<SparseSimpleQvRecursor>;
    %template(SparseSseQvRecursor)            SseRecursor<SparseMatrix, QvEvaluator, detail::ViterbiCombiner>;
    %template(SparseSseQvMutationScorer)      MutationScorer<SparseSseQvRecursor>;

    %template(SparseSseQvMultiReadMutationScorer) MultiReadMutationScorer<SparseSseQvRecursor>;

	//
	// Edna evaluator support
	//
    %template(SparseEdnaRecursorBase)          detail::RecursorBase<SparseMatrix, EdnaEvaluator, detail::SumProductCombiner>;
    %template(SparseSseEdnaRecursor)            SseRecursor<SparseMatrix, EdnaEvaluator, detail::SumProductCombiner>;
    %template(SparseSseEdnaMutationScorer)      MutationScorer<SparseSseEdnaRecursor>;
}
