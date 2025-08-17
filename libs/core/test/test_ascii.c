#include "core_ascii.h"

#include "anvil_spec.h"

spec(ascii) {

    it("can verify validity") {
        anvil(ascii_is_valid('a'));
        anvil(!ascii_is_valid(200));
    }

    it("can check if a character is a digit") {
        anvil(ascii_is_digit('1'));
        anvil(ascii_is_digit('0'));
        anvil(!ascii_is_digit('a'));
    }

    it("can check if a character is a hex digit") {
        anvil(ascii_is_hex_digit('1'));
        anvil(ascii_is_hex_digit('0'));
        anvil(ascii_is_hex_digit('a'));
        anvil(ascii_is_hex_digit('a'));
        anvil(ascii_is_hex_digit('F'));
        anvil(ascii_is_hex_digit('f'));
        anvil(!ascii_is_hex_digit('z'));
    }

    it("can check if a character is a letter") {
        anvil(ascii_is_letter('a'));
        anvil(ascii_is_letter('z'));
        anvil(ascii_is_letter('B'));
        anvil(!ascii_is_letter('5'));
    }

    it("can check if a character is a lower-case letter") {
        anvil(ascii_is_lower('a'));
        anvil(ascii_is_lower('z'));
        anvil(!ascii_is_lower('B'));
        anvil(!ascii_is_lower('5'));
    }

    it("can check if a character is a upper-case letter") {
        anvil(ascii_is_upper('A'));
        anvil(ascii_is_upper('Z'));
        anvil(!ascii_is_upper('b'));
        anvil(!ascii_is_upper('5'));
    }

    it("can check if a character is a control character") {
        anvil(ascii_is_control('\t'));
        anvil(ascii_is_control('\a'));
        anvil(!ascii_is_control('A'));
        anvil(!ascii_is_control('Z'));
        anvil(!ascii_is_control('b'));
        anvil(!ascii_is_control('5'));
    }

    it("can check if a character is a whitespace character") {
        anvil(ascii_is_whitespace(' '));
        anvil(ascii_is_whitespace('\n'));
        anvil(ascii_is_whitespace('\t'));
        anvil(!ascii_is_whitespace('Z'));
        anvil(!ascii_is_whitespace('b'));
        anvil(!ascii_is_whitespace('5'));
    }

    it("can check if a character is a newline character") {
        anvil(ascii_is_newline('\n'));
        anvil(ascii_is_newline('\r'));
        anvil(!ascii_is_newline('Z'));
        anvil(!ascii_is_newline('b'));
        anvil(!ascii_is_newline('5'));
    }

    it("can check if a character is printable") {
        anvil(ascii_is_printable(' '));
        anvil(ascii_is_printable('Z'));
        anvil(ascii_is_printable('b'));
        anvil(ascii_is_printable('5'));
        anvil(!ascii_is_printable('\n'));
        anvil(!ascii_is_printable('\r'));
        anvil(!ascii_is_printable('\a'));
    }

    it("can toggle the casing of a character") {
        anvil(ascii_toggle_case('a') == 'A');
        anvil(ascii_toggle_case('A') == 'a');
    }

    it("can convert a character to upper-case") {
        anvil(ascii_to_upper('a') == 'A');
        anvil(ascii_to_upper('A') == 'A');
    }

    it("can convert a character to lower-case") {
        anvil(ascii_to_lower('A') == 'a');
        anvil(ascii_to_lower('a') == 'a');
    }

    it("can convert a character to an integer") {
        anvil(ascii_to_integer('0') == 0);
        anvil(ascii_to_integer('5') == 5);
        anvil(ascii_to_integer('9') == 9);
        anvil(ascii_to_integer('a') == 10);
        anvil(ascii_to_integer('A') == 10);
        anvil(ascii_to_integer('c') == 12);
        anvil(ascii_to_integer('C') == 12);
        anvil(ascii_to_integer('f') == 15);
        anvil(ascii_to_integer('F') == 15);
        anvil(sentinel_check(ascii_to_integer(' ')));
        anvil(sentinel_check(ascii_to_integer('\b')));
    }
}