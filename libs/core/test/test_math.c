#include "core_math.h"

#include "anvil_spec.h"

spec(math) {

    it("can compute the min argument") {
        anvil_eq_int(math_min(1, 0), 0);
        anvil_eq_int(math_min(0, 0), 0);
        anvil_eq_int(math_min(1, -1), -1);
        anvil_eq_int(math_min(-1, 0), -1);

        anvil_eq_float(math_min(-1.0f, 0.0f), -1.0f, 1e-6);
        anvil_eq_float(math_min(-1.1f, -1.2f), -1.2f, 1e-6);
    }

    it("can compute the max argument") {
        anvil_eq_int(math_max(1, 0), 1);
        anvil_eq_int(math_max(0, 0), 0);
        anvil_eq_int(math_max(-1, 1), 1);
        anvil_eq_int(math_max(-1, -2), -1);

        anvil_eq_float(math_max(-1.0f, 0.1f), 0.1f, 1e-6);
        anvil_eq_float(math_max(-1.1f, -1.2f), -1.1f, 1e-6);
    }

    it("can compute the sign of the argument") {
        anvil_eq_int(math_sign(-42), -1);
        anvil_eq_int(math_sign(42), 1);
        anvil_eq_int(math_sign(0), 0);

        anvil_eq_int(math_sign(-0.1f), -1);
        anvil_eq_int(math_sign(0.1f), 1);
        anvil_eq_int(math_sign(0.0f), 0);
    }

    it("can compute the absolute of the argument") {
        anvil_eq_int(math_abs(-42), 42);
        anvil_eq_int(math_abs(42), 42);
        anvil_eq_int(math_abs(0), 0);
        anvil_eq_float(math_abs(-1.25), 1.25, 1e-6);
        anvil_eq_float(math_abs(0.0), 0.0, 1e-6);
    }

    it("can truncate the fractional part of floats") {
        anvil_eq_float(math_trunc_f64(1.42), 1.0, 1e-24);
        anvil_eq_float(math_trunc_f64(42.1337), 42.0, 1e-24);
        anvil_eq_float(math_trunc_f64(-1.42), -1.0, 1e-24);
        anvil_eq_float(math_trunc_f64(-42.1337), -42.0, 1e-24);
        anvil_eq_float(math_trunc_f64(-.34), 0.0, 1e-24);
    }

    it("can floor (round-down) floats") {
        anvil_eq_float(math_floor_f64(.1), 0.0, 1e-24);
        anvil_eq_float(math_floor_f64(1.1), 1.0, 1e-24);
        anvil_eq_float(math_floor_f64(1.99), 1.0, 1e-24);
        anvil_eq_float(math_floor_f64(-42.1337), -43.0, 1e-24);
        anvil_eq_float(math_floor_f64(-2.3), -3.0, 1e-24);
        anvil_eq_float(math_floor_f64(-1.99), -2.0, 1e-24);
    }

    it("can ceil (round-up) floats") {
        anvil_eq_float(math_ceil_f64(1.0), 1.0, 1e-24);
        anvil_eq_float(math_ceil_f64(0.0), 0.0, 1e-24);
        anvil_eq_float(math_ceil_f64(1.2), 2.0, 1e-24);
        anvil_eq_float(math_ceil_f64(-1.0), -1.0, 1e-24);
        anvil_eq_float(math_ceil_f64(-1.2), -1.0, 1e-24);
        anvil_eq_float(math_ceil_f64(-42.1337), -42.0, 1e-24);
        anvil_eq_float(math_ceil_f64(-1.99), -1.0, 1e-24);
        anvil_eq_float(math_ceil_f64(-1.01), -1.0, 1e-24);
    }

    it("can round (round to even) floats") {
        anvil_eq_float(math_round_f64(1.0), 1.0, 1e-24);
        anvil_eq_float(math_round_f64(.0), 0.0, 1e-24);
        anvil_eq_float(math_round_f64(.6), 1.0, 1e-24);
        anvil_eq_float(math_round_f64(.5), 0.0, 1e-24);
        anvil_eq_float(math_round_f64(.499), 0.0, 1e-24);
        anvil_eq_float(math_round_f64(.51), 1.0, 1e-24);
        anvil_eq_float(math_round_f64(1.4), 1.0, 1e-24);
        anvil_eq_float(math_round_f64(1.5), 2.0, 1e-24);
        anvil_eq_float(math_round_f64(1.6), 2.0, 1e-24);
        anvil_eq_float(math_round_f64(2.5), 2.0, 1e-24);
        anvil_eq_float(math_round_f64(2.6), 3.0, 1e-24);
        anvil_eq_float(math_round_f64(3.5), 4.0, 1e-24);
        anvil_eq_float(math_round_f64(-.1), 0.0, 1e-24);
        anvil_eq_float(math_round_f64(-.4), 0.0, 1e-24);
        anvil_eq_float(math_round_f64(-.5), 0.0, 1e-24);
        anvil_eq_float(math_round_f64(-1.5), -2.0, 1e-24);
        anvil_eq_float(math_round_f64(-1.6), -2.0, 1e-24);
        anvil_eq_float(math_round_f64(-2.5), -2.0, 1e-24);
        anvil_eq_float(math_round_f64(-2.6), -3.0, 1e-24);
        anvil_eq_float(math_round_f64(-3.5), -4.0, 1e-24);
    }
}
