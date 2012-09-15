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

#include <algorithm>
#include <boost/range.hpp>
#include <boost/shared_array.hpp>
#include <boost/utility.hpp>
#include <cassert>
#include <string>
#include <vector>

#include "Types.hpp"

namespace ConsensusCore
{
    // Feature/Features object usage caveats:
    //  - Feature and Features objects _must_ be stored by value, not reference
    //  - The underlying array must be allocated using new[]
    template <typename T>
    class Feature : private boost::shared_array<T>
    {
    public:
        // \brief Allocate a new feature object, copying content from ptr.
        Feature(const T* ptr, int length)
            : boost::shared_array<T>(new T[length]),
              length_(length)
        {
            assert(length >= 0);
            std::copy(ptr, ptr + length, get());
        }

        // \brief Allocate and zero-fill a new feature object of given length.
        explicit Feature(int length)
            : boost::shared_array<T>(new T[length]()),
              length_(length)
        {
            assert(length >= 0);
        }

        int Length() const
        {
            return length_;
        }

        const T& operator[](int i) const
        {
            return this->boost::shared_array<T>::operator[](i);
        }

        T& operator[](int i)
        {
            return this->boost::shared_array<T>::operator[](i);
        }

        T ElementAt(int i) const
        {
            return (*this)[i];
        }

    private:
        int length_;

#ifndef SWIG
    public:
        T* get()
        {
            return this->boost::shared_array<T>::get();
        }

        const T* get() const
        {
            return this->boost::shared_array<T>::get();
        }

        operator std::string() const;
#endif  // !SWIG
    };


#ifndef SWIG
    //
    // Support for boost::foreach
    //
    template<typename T>
    inline const T* range_begin(const Feature<T>& f)
    {
        return f.get();
    }

    template<typename T>
    inline const T* range_end(const Feature<T>& f)
    {
        return f.get() + f.Length();
    }

    template<typename T>
    inline T* range_begin(Feature<T>& f) // NOLINT
    {
        return f.get();
    }

    template<typename T>
    inline T* range_end(Feature<T>& f)  // NOLINT
    {
        return f.get() + f.Length();
    }
#endif  // !SWIG

    typedef Feature<float> FloatFeature;
    typedef Feature<char> CharFeature;
    typedef Feature<int> IntFeature;
}


#ifndef SWIG
namespace boost
{
    template<typename T>
    struct range_const_iterator<ConsensusCore::Feature<T> >
    {
        typedef const T* type;
    };

    template<typename T>
    struct range_mutable_iterator<ConsensusCore::Feature<T> >
    {
        typedef T* type;
    };
}
#endif  // !SWIG
