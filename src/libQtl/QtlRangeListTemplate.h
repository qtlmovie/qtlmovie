//----------------------------------------------------------------------------
//
// Copyright (c) 2016, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//
// Qtl, Qt utility library.
// Template methods for class QtlRangeList.
//
//----------------------------------------------------------------------------

#ifndef QTLRANGELISTTEMPLATE_H
#define QTLRANGELISTTEMPLATE_H


//----------------------------------------------------------------------------
// Merge all overlapping or adjacent segments.
//----------------------------------------------------------------------------

template<typename INT>
void QtlRangeList<INT>::merge(Qtl::MergeRangeFlags flags)
{
    // Sort the list first if required.
    if (flags & Qtl::Sorted) {
        this->sort();
    }

    typename SuperClass::Iterator it(this->begin());
    if (flags & Qtl::NoDuplicate) {
        // Remove duplicates, merge overlapped ranges.
        while (it != this->end()) {
            typename SuperClass::Iterator next(it + 1);
            if (next != this->end() && (it->overlap(*next) || it->adjacent(*next))) {
                it->merge(*next);
                it = this->erase(next) - 1;
                // If not sorted, the merge may have set the first value at iterator backward.
                // We may need to merge backward.
                if (!(flags & Qtl::Sorted) && it != this->begin()) {
                    --it;
                }
            }
            else {
                it = next;
            }

        }
    }
    else {
        // Only merge exactly adjacent ranges.
        while (it != this->end()) {
            typename SuperClass::Iterator next(it + 1);
            if (next != this->end() && it->adjacent(*next, Qtl::AdjacentBefore)) {
                it->merge(*next);
                it = this->erase(next) - 1;
            }
            else {
                it = next;
            }
        }
    }
}

#endif // QTLRANGELISTTEMPLATE_H
