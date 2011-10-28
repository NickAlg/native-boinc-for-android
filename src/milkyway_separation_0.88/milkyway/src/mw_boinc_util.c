/* Copyright 2010 Matthew Arsenault, Travis Desell, Boleslaw
Szymanski, Heidi Newberg, Carlos Varela, Malik Magdon-Ismail and
Rensselaer Polytechnic Institute.

This file is part of Milkway@Home.

Milkyway@Home is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Milkyway@Home is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Milkyway@Home.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "milkyway_util.h"
#include "milkyway_cpp_util.h"
#include "mw_boinc_util.h"

#ifndef _WIN32
  #include <sys/time.h>
#endif

#if BOINC_APPLICATION
  #include <diagnostics.h>
#endif

#include <sys/stat.h>


#if BOINC_APPLICATION

static const int debugOptions = BOINC_DIAG_DUMPCALLSTACKENABLED
                              | BOINC_DIAG_HEAPCHECKENABLED
                              | BOINC_DIAG_MEMORYLEAKCHECKENABLED;

/* I don't understand why the graphics have a separate debug with
 * diagnostics API type stuff. */
static int mwBoincInitGraphics(int useDebug)
{
    return boinc_init_graphics_diagnostics(useDebug ? debugOptions : BOINC_DIAG_DEFAULTS);
}

static int mwBoincInitNormal(MWInitType type)
{
    int rc = 0;
    BOINC_OPTIONS options;

    if (type & MW_DEBUG)
    {
        rc |= boinc_init_diagnostics(debugOptions);
    }

    mwGetBoincOptionsDefault(&options);

    //options.multi_thread = (type & MW_MULTITHREAD) > 0;
    options.normal_thread_priority = ((type & MW_CAL) || (type & MW_OPENCL)) > 0;

    rc |= boinc_init_options(&options);

    return rc;
}


int mwBoincInit(MWInitType type)
{
    int rc = 0;

    if (type & MW_GRAPHICS)
    {
        rc = mwBoincInitGraphics(type);
    }
    else
    {
        rc = mwBoincInitNormal(type);
    }

    if (rc)
        warn("Failed to init BOINC\n");

    return rc;
}

FILE* mwOpenResolved(const char* filename, const char* mode)
{
    int ret;
    char resolvedPath[1024];

    ret = boinc_resolve_filename(filename, resolvedPath, sizeof(resolvedPath));
    if (ret)
    {
        warn("Error resolving file '%s': %d\n", filename, ret);
        return NULL;
    }

    return mw_fopen(resolvedPath, mode);
}

char* mwReadFileResolved(const char* filename)
{
    return mwFreadFile(mwOpenResolved(filename, "rb"), filename);
}

int mw_resolve_filename(const char* filename, char* buf, size_t bufSize)
{
    return boinc_resolve_filename(filename, buf, bufSize);
}

int mw_file_exists(const char* file)
{
    return boinc_file_exists(file);
}

int mw_rename(const char* oldf, const char* newf)
{
    return boinc_rename(oldf, newf);
}

#else /* !BOINC_APPLICATION */

int mwBoincInit(MWInitType type)
{
    return 0;
}

FILE* mwOpenResolved(const char* filename, const char* mode)
{
    return mw_fopen(filename, mode);
}

char* mwReadFileResolved(const char* filename)
{
    return mwReadFile(filename);
}

int mw_resolve_filename(const char* filename, char* buf, size_t bufSize)
{
    int rc;

    assert(buf != filename);
    rc = snprintf(buf, bufSize, "%s", filename);
    return (rc == -1) || ((size_t) rc == bufSize);
}

int mw_file_exists(const char* file)
{
    struct stat statBuf;
    return !stat(file, &statBuf);
}

#ifndef _WIN32

int mw_rename(const char* oldf, const char* newf)
{
    return rename(oldf, newf);
}

#else

/* It turns out that rename() does exist although it doesn't behave
properly and errors if the destination file already exists which is
wrong. This isn't quite atomic like it's supposed to be. */

int mw_rename(const char* oldf, const char* newf)
{
    if (MoveFileExA(oldf, newf, MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
        return 0;
    return GetLastError();
}
#endif /* _WIN32 */

#endif /* BOINC_APPLICATION */


