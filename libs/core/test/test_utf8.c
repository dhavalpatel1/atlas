#include "core_utf8.h"

#include "anvil_spec.h"

spec(utf8) {

    static String testStr =
        string_static("STARGΛ̊TE,Hello world,Καλημέρα κόσμε,コンニチハ,⡌⠁⠧⠑ ⠼⠁⠒,ᚻᛖ ᚳᚹᚫᚦ ᚦᚫᛏ,ሰማይ አይታረስ "
                      "ንጉሥ አይከሰስ።,แผ่นดินฮั่นเสื่อมโทรมแสนสังเวช,Зарегистрируйтесь,გთხოვთ ახლავე გაიაროთ⎪⎢⎜ "
                      "⎳aⁱ-bⁱ⎟⎥⎪▁▂▃▄▅▆▇█∀∂∈ℝ∧∪≡∞");

    it("can count codepoints in a utf8 string") {
        anvil_eq_int(utf8_cp_count(string_lit("")), 0);
        anvil_eq_int(utf8_cp_count(string_lit("Hello")), 5);
        anvil_eq_int(utf8_cp_count(testStr), 184);
    }
}