#pragma once

#include "core_types.h"

#define math_min(_A_, _B_) ((_A_) < (_B_) ? (_A_) : (_B_))
#define math_max(_A_, _B_) ((_A_) > (_B_) ? (_A_) : (_B_))

#define math_sign(_A_) (((_A_) > 0) - ((_A_) < 0))

#define math_abs(_A_) ((_A_) < 0 ? -(_A_) : (_A_))

#define math_pi_f32 3.141592653589793238463f
#define math_pi_f64 3.141592653589793238463

u64 math_pow10_u64(u8 n);

f32 math_sqrt_f32(f32 x);

f32 math_log_f32(f32 x);

f32 math_sin_f32(f32 x);

f32 math_cos_f32(f32 x);