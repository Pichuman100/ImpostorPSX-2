/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef PSXF_GUARD_MUTIL_H
#define PSXF_GUARD_MUTIL_H

#include "psx.h"

#include "fixed.h"

//Math utility functions
#define lerp(position, target, speed) position + FIXED_MUL((target - position),speed)

s16 MUtil_Sin(u8 x);
s16 MUtil_Cos(u8 x);
void MUtil_RotatePoint(POINT *p, s16 s, s16 c);
fixed_t MUtil_Pull(fixed_t a, fixed_t b, fixed_t t);
fixed_t fixed_lerp(fixed_t position, fixed_t target, fixed_t speed);

#endif
