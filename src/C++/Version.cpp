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

#include <string>
#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#include "Version.hpp"

namespace ConsensusCore
{
    using boost::make_tuple;
    using boost::format;
    using boost::str;

    int Version::Major()
    {
        return API_MAJOR;
    }

    int Version::Minor()
    {
        return API_MINOR;
    }

    int Version::Patch()
    {
        return API_PATCH;
    }

    std::string Version::VersionString()
    {
        std::string version = str(format("%d.%d.%d") % API_MAJOR % API_MINOR % API_PATCH);
#ifndef NDEBUG
        version += "-DEBUG";
#endif  // !NDEBUG
        return version;
    }

    bool Version::IsAtLeast(int major, int minor, int patch)
    {
        // lexicographical comparison using boost tuples
        return make_tuple(major, minor, patch) >= make_tuple(API_MAJOR, API_MINOR, API_PATCH);
    }

    bool Version::HasFeature(const std::string& name)
    {
        return false;
    }
}
