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

#include "nbody_priv.h"
#include "nbody_orbit_integrator.h"
#include "nbody_potential.h"

/* Simple orbit integrator in user-defined potential
    Written for BOINC Nbody
    willeb 10 May 2010 */
void reverseOrbit(mwvector* finalPos,
                  mwvector* finalVel,
                  const Potential* pot,
                  mwvector pos,
                  mwvector vel,
                  real tstop,
                  real dt)
{
    mwvector acc, v, x;
    real t;

    // Set the initial conditions
    x = pos;
    v = vel;
    mw_incnegv(v);

    // Get the initial acceleration
    acc = acceleration(pot, x);

    // Loop through time
    for (t = 0; t <= tstop; t += dt)
    {
        // Update the velocities and positions
        mw_incaddv_s(v, acc, dt);
        mw_incaddv_s(x, v, dt);

        // Compute the new acceleration
        acc = acceleration(pot, x);
    }

    /* Report the final values (don't forget to reverse the velocities) */
    mw_incnegv(v);

    *finalPos = x;
    *finalVel = v;
}

