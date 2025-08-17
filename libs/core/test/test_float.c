#include "core_float.h"
#include "core_bits.h"

#include "anvil_spec.h"

spec(float) {
    it("can detect a NaN float") {
        anvil(float_isnan(f32_nan));
        anvil(float_isnan(f64_nan));
    }

    it("can detect an infinite float") {
        anvil(float_isinf(f32_inf));
        anvil(float_isinf(f64_inf));
    }
}