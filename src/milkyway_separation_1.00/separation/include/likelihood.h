/*
 *  Copyright (c) 2008-2010 Travis Desell, Nathan Cole
 *  Copyright (c) 2008-2010 Boleslaw Szymanski, Heidi Newberg
 *  Copyright (c) 2008-2010 Carlos Varela, Malik Magdon-Ismail
 *  Copyright (c) 2008-2011 Rensselaer Polytechnic Institute
 *  Copyright (c) 2010-2011 Matthew Arsenault
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

#ifndef _LIKELIHOOD_H_
#define _LIKELIHOOD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "separation_types.h"

int likelihood(SeparationResults* results,
               const AstronomyParameters* ap,
               const StarPoints* sp,
               const StreamConstants* sc,
               const Streams* streams,
               const StreamGauss sg,
               const int do_separation,
               const char* separation_outfile);

#ifdef ANDROID
int likelihood_intfp(SeparationResults* results,
                    const AstronomyParameters* ap,
                    const StarPoints* sp,
                    const StreamConstantsIntFp* sc,
                    const Streams* streams,
                    const StreamGauss sg,
                    const int do_separation,
                    const char* separation_outfile);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _LIKELIHOOD_H_ */

