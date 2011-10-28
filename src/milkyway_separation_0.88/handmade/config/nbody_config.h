/* Copyright 2010 Matthew Arsenault, Travis Desell, Dave Przybylo,
Nathan Cole, Boleslaw Szymanski, Heidi Newberg, Carlos Varela, Malik
Magdon-Ismail and Rensselaer Polytechnic Institute.

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

#ifndef _NBODY_CONFIG_H_
#define _NBODY_CONFIG_H_

#include "milkyway_config.h"

#define MILKYWAY_NBODY_VERSION_MAJOR 0
#define MILKYWAY_NBODY_VERSION_MINOR 51
#define MILKYWAY_NBODY_VERSION       0.51

#define NBODY_OPENCL 0
#define NBODY_CRLIBM 1

#define ENABLE_CRLIBM NBODY_CRLIBM
#define ENABLE_OPENCL NBODY_OPENCL

#if defined(_OPENMP)
  #define EXTRAVER " OpenMP"
#elif NBODY_OPENCL
  #define EXTRAVER " OpenCL"
#else
  #define EXTRAVER
#endif /* defined(_OPENMP) */

#if ENABLE_CRLIBM
  #define EXTRALIB " Crlibm"
#else
  #define EXTRALIB
#endif


#if !BOINC_APPLICATION && !defined(_WIN32)
  #define USE_SHMEM 1
#elif BOINC_APPLICATION
  #define USE_BOINC_SHMEM 1
#endif

#define NBODY_BIN_NAME ""
#define NBODY_GRAPHICS_NAME ""


#define BOINC_NBODY_APP_VERSION "milkywayathome nbody 0.51 Linux " ARCH_STRING " " PRECSTRING EXTRAVER EXTRALIB

#define DEFAULT_OUTPUT_FILE stderr

#endif /* NBODY_CONFIG_H_ */

