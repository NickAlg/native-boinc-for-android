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

#ifndef _PROBABILITIES_H_
#define _PROBABILITIES_H_

#include "separation_types.h"
#include "evaluation_state.h"
#include "evaluation.h"
#include "probabilities_dispatch.h"

#ifdef __cplusplus
extern "C" {
#endif

/* probabilities will be rebuilt for each SSE level */
#if MW_IS_X86
  #if defined(__AVX__)
    #define INIT_PROBABILITIES initProbabilities_AVX
  #elif defined(__SSE4_1__)
    #define INIT_PROBABILITIES initProbabilities_SSE41
  #elif defined(__SSE3__)
    #define INIT_PROBABILITIES initProbabilities_SSE3
  #elif defined(__SSE2__)
    #define INIT_PROBABILITIES initProbabilities_SSE2
  #else
    #define INIT_PROBABILITIES initProbabilities
  #endif
#else
  #define INIT_PROBABILITIES initProbabilities
#endif /* MW_IS_X86 */


#if MW_IS_X86
ProbabilityFunc initProbabilities_AVX(void);
ProbabilityFunc initProbabilities_SSE41(void);
ProbabilityFunc initProbabilities_SSE3(void);
ProbabilityFunc initProbabilities_SSE2(void);
#endif /* MW_IS_X86 */


real probabilities_fast_hprob(const AstronomyParameters* ap,
                              const StreamConstants* sc,
                              const real* RESTRICT sg_dx,
                              const real* RESTRICT r_point,
                              const real* RESTRICT qw_r3_N,
                              LBTrig lbt,
                              real gPrime,
                              real reff_xr_rp3,
                              real* RESTRICT streamTmps);

real probabilities_slow_hprob(const AstronomyParameters* ap,
                              const StreamConstants* sc,
                              const real* RESTRICT sg_dx,
                              const real* RESTRICT r_point,
                              const real* RESTRICT qw_r3_N,
                              LBTrig lbt,
                              real gPrime,
                              real reff_xr_rp3,
                              real* RESTRICT streamTmps);

#ifdef ANDROID
real prob_arm_novfp(const AstronomyParameters* ap, const StreamConstants* sc,
        const real* RESTRICT sg_dx, const real* RESTRICT r_point,
        const real* RESTRICT qw_r3_N, LBTrig lbt, real gPrime, real reff_xr_rp3,
        real* RESTRICT streamTmps);

real prob_arm_novfp_debug(const AstronomyParameters* ap, const StreamConstants* sc,
        const real* RESTRICT sg_dx, const real* RESTRICT r_point,
        const real* RESTRICT qw_r3_N, LBTrig lbt, real gPrime, real reff_xr_rp3,
        real* RESTRICT streamTmps);

real prob_arm_vfp(const AstronomyParameters* ap, const StreamConstants* sc,
        const real* RESTRICT sg_dx, const real* RESTRICT r_point,
        const real* RESTRICT qw_r3_N, LBTrig lbt, real gPrime, real reff_xr_rp3,
        real* RESTRICT streamTmps);

real prob_arm_vfpv3(const AstronomyParameters* ap, const StreamConstants* sc,
        const real* RESTRICT sg_dx, const real* RESTRICT r_point,
        const real* RESTRICT qw_r3_N, LBTrig lbt, real gPrime, real reff_xr_rp3,
        real* RESTRICT streamTmps);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _PROBABILITIES_H_ */

