/*
 *  Copyright (c) 2010-2011 Matthew Arsenault
 *  Copyright (c) 2010-2011 Rensselaer Polytechnic Institute
 *
 *  This file is part of Milkway@Home.
 *
 *  Milkway@Home is free software: you may copy, redistribute and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "milkyway_util.h"
#include "milkyway_alloc.h"

#ifndef _WIN32
  #include <sys/time.h>
  #include <sys/resource.h>
#endif /* _WIN32 */

#if HAVE_MALLOC_H
  #include <malloc.h>
#endif

#if HAVE_WINDOWS_H
  #include <windows.h>
#endif


void* mwCalloc(size_t count, size_t size)
{
    void* mem = (void*) calloc(count, size);
    if (mem == NULL)
        mw_fail("calloc failed: "ZU" bytes\n", count * size);
    return mem;
}

void* mwMalloc(size_t size)
{
    void* mem = (void*) malloc(size);
    if (mem == NULL)
        mw_fail("malloc failed: "ZU" bytes\n", size);
    return mem;
}


#if HAVE_POSIX_MEMALIGN

void* mwMallocA(size_t size)
{
    void* p;

    if (posix_memalign(&p, 32, size))
    {
        mw_fail("Failed to allocate block of size %zu aligned to 16\n", size);
    }

    if (!p)
        mw_fail("%s: NULL\n", __func__);

    return p;
}

#elif HAVE__ALIGNED_MALLOC || HAVE___MINGW_ALIGNED_MALLOC

void* mwMallocA(size_t size)
{
    void* p;

    p = _aligned_malloc(size, 32);
    if (!p)
        mw_fail("_aligned_malloc error = %ld\n", GetLastError());

    return p;
}

#elif HAVE_MEMALIGN

void* mwMallocA(size_t size)
{
    void* p;

    if ((p = memalign(16, size)) == NULL)
    {
        mw_fail("Failed to allocate block of size %zu aligned to 16\n", size);
    }

    if (!p)
        mw_fail("%s: NULL\n", __func__);

    return p;
}

#else

/* OS X already aligns everything to 16 bytes. posix_memalign() does not exist pre 10.6 */
void* mwMallocA(size_t size)
{
    return mwMalloc(size);
}

#endif /* HAVE_POSIX_MEMALIGN */

void* mwRealloc(void* ptr, size_t size)
{
    void* mem = (void*) realloc(ptr, size);
    if (mem == NULL)
        mw_fail("realloc failed: "ZU" bytes\n", size);
    return mem;
}


/* Return true if we can be sure our alignment is 32 aligned */
int mwAllocA32Safe()
{
    return (HAVE_POSIX_MEMALIGN || HAVE___MINGW_ALIGNED_MALLOC || HAVE__ALIGNED_MALLOC);
}

/* Return true if we can be sure our alignment is 32 aligned */
int mwAllocA16Safe()
{
  #ifdef __APPLE__
    return TRUE;
  #endif

    return (HAVE_POSIX_MEMALIGN || HAVE___MINGW_ALIGNED_MALLOC || HAVE__ALIGNED_MALLOC);
}


#ifndef __APPLE__

void* mwCallocA(size_t count, size_t size)
{
    void* p;
    size_t totalSize = count * size;

    p = mwMallocA(totalSize);
    memset(p, 0, totalSize);

    return p;
}

#else
void* mwCallocA(size_t count, size_t size)
{
    return mwCalloc(count, size);
}

#endif


