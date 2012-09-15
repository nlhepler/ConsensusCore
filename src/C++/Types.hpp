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

#pragma once
#include <string>

//
// Forward declarations
//
namespace ConsensusCore {
    class BandingOptions;
    class DenseMatrix;
    class QuiverConfig;
    class PairwiseAlignment;
    class AnnotatedPairwiseAlignment;
    class PoaConfig;
    class QvModelParams;
    class EdnaModelParams;
    class ChannelSequenceFeatures;
    class QvSequenceFeatures;
    class SequenceFeatures;
    class SparseMatrix;
    class Mutation;
}

namespace ConsensusCore {
namespace detail {
    class ViterbiCombiner;
    class SumProductCombiner;
}}


//
// Exception types
//

namespace ConsensusCore {
    /// \brief Abstract base class for "error"-type exceptions.  Do
    ///        not catch these.
    class ErrorBase
    {
    public:
        virtual std::string Message() const throw() = 0;
        virtual ~ErrorBase() {}
    };


    /// \brief Abstract base class for exceptions, which user code
    ///        may safely catch.
    class ExceptionBase
    {
    public:
        virtual std::string Message() const throw() = 0;
        virtual ~ExceptionBase() {}
    };


    /// \brief An exception signaling an error in ConsensusCore's internal logic.
    class InternalError : public ErrorBase
    {
    public:
        InternalError()
            : msg_("Internal error encountered!")
        {}

        explicit InternalError(const std::string& msg)
            : msg_(msg)
        {}

        std::string Message() const throw()
        {
            return msg_;
        }

    private:
        std::string msg_;
    };

    class InvalidInputError : public ErrorBase
    {
    public:
        InvalidInputError()
            : msg_("Invalid input!")
        {}

        explicit InvalidInputError(const std::string& msg)
            : msg_(msg)
        {}

        std::string Message() const throw()
        {
            return msg_;
        }

    private:
        std::string msg_;
    };

    class NotYetImplementedException : public ErrorBase
    {
    public:
        std::string Message() const throw()
        {
            return "Feature not yet implemented";
        }
    };
}

