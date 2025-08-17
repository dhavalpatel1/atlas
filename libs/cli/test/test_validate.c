#include "cli_validate.h"

#include "anvil_spec.h"

spec(validate) {
    it("supports validating i64's") {
        anvil(cli_validate_i64(string_lit("42")));
        anvil(cli_validate_i64(string_lit("-42")));

        anvil(!cli_validate_i64(string_lit("Hello")));
        anvil(!cli_validate_i64(string_lit("--42")));
    }

    it("supports validating u64's") {
        anvil(cli_validate_u64(string_lit("42")));

        anvil(!cli_validate_u64(string_lit("Hello")));
        anvil(!cli_validate_u64(string_lit("-42")));
    }

    it("supports validating f64's") {
        anvil(cli_validate_f64(string_lit("42.1337e-2")));

        anvil(!cli_validate_f64(string_lit("Hello")));
        anvil(!cli_validate_f64(string_lit("42.1337f-2")));
    }
}