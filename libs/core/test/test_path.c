#include "core_alloc.h"
#include "core_array.h"
#include "core_path.h"
#include "core_rng.h"

#include "anvil_spec.h"
#include "core_string.h"

spec(path) {
    it("can check if a path is absolute") {
        anvil(path_is_absolute(string_lit("/")));
        anvil(path_is_absolute(string_lit("c:/")));
        anvil(path_is_absolute(string_lit("C:/")));
        anvil(path_is_absolute(string_lit("C:\\")));

        anvil(!path_is_absolute(string_lit("Hello")));
        anvil(!path_is_absolute(string_lit("./")));
        anvil(!path_is_absolute(string_lit("../")));
        anvil(!path_is_absolute(string_lit("\\")));
    }

    it("can check if a path is a root") {
        anvil(path_is_root(string_lit("/")));
        anvil(path_is_root(string_lit("c:/")));
        anvil(path_is_root(string_lit("C:/")));
        anvil(path_is_root(string_lit("C:\\")));

        anvil(!path_is_root(string_lit("Hello")));
        anvil(!path_is_root(string_lit("/Hello")));
        anvil(!path_is_root(string_lit("c:/Hello")));
    }

    it("can retrieve the file-name of a path") {
        anvil_eq_string(path_filename(string_lit("note.txt")), string_lit("note.txt"));
        anvil_eq_string(path_filename(string_lit("/stuff/note.txt")), string_lit("note.txt"));
        anvil_eq_string(path_filename(string_lit("c:/stuff/note.txt")), string_lit("note.txt"));
        anvil_eq_string(path_filename(string_lit("c:/stuff/")), string_empty);
        anvil_eq_string(path_filename(string_lit("/")), string_empty);
    }

    it("can retrieve the extension of a path") {
        anvil_eq_string(path_extension(string_lit("note.txt")), string_lit("txt"));
        anvil_eq_string(path_extension(string_lit("note.txt.back")), string_lit("back"));
        anvil_eq_string(path_extension(string_lit("c:/.stuff/note.txt")), string_lit("txt"));
        anvil_eq_string(path_extension(string_lit("c:/.stuff/note")), string_empty);
        anvil_eq_string(path_extension(string_lit("c:/.stuff/note.")), string_empty);
        anvil_eq_string(path_extension(string_lit("c:/.stuff/.")), string_empty);
        anvil_eq_string(path_extension(string_lit("c:/.stuff/..")), string_empty);
    }

    it("can retrieve the stem of a path") {
        anvil_eq_string(path_stem(string_lit("note.txt")), string_lit("note"));
        anvil_eq_string(path_stem(string_lit("note.txt.back")), string_lit("note"));
        anvil_eq_string(path_stem(string_lit("note")), string_lit("note"));
        anvil_eq_string(path_stem(string_lit("note.")), string_lit("note"));
        anvil_eq_string(path_stem(string_lit("c:/.stuff/note.txt")), string_lit("note"));
        anvil_eq_string(path_stem(string_lit("c:/.stuff/.")), string_empty);
    }

    it("can retrieve the parent of a path") {
        anvil_eq_string(path_parent(string_lit("How/You/Doing")), string_lit("How/You"));
        anvil_eq_string(path_parent(string_lit("stuff")), string_lit(""));
        anvil_eq_string(path_parent(string_lit("stuff/")), string_lit("stuff"));
        anvil_eq_string(path_parent(string_lit("c:/stuff")), string_lit("c:/"));
        anvil_eq_string(path_parent(string_lit("c:/stuff/note.txt")), string_lit("c:/stuff"));
        anvil_eq_string(path_parent(string_lit("c:/")), string_lit("c:/"));
        anvil_eq_string(path_parent(string_lit("/")), string_lit("/"));
        anvil_eq_string(path_parent(string_lit("/Stuff")), string_lit("/"));
    }

    it("can canonize paths") {
        struct {
            String path;
            String expected;
        } const data[] = {
            {string_lit("/"), string_lit("/")},
            {string_lit("/Hello World"), string_lit("/Hello World")},
            {string_lit("C:\\"), string_lit("C:/")},
            {string_lit("C:/"), string_lit("C:/")},
            {string_lit("c:\\"), string_lit("C:/")},
            {string_lit("c:/"), string_lit("C:/")},
            {string_lit("c:\\Hello World"), string_lit("C:/Hello World")},
            {string_lit("/How/You/Doing"), string_lit("/How/You/Doing")},
            {string_lit("How/You/Doing"), string_lit("How/You/Doing")},
            {string_lit("How/You/Doing/"), string_lit("How/You/Doing")},
            {string_lit("How/You/Doing//"), string_lit("How/You/Doing")},
            {string_lit(".How/..You/...Doing/."), string_lit(".How/..You/...Doing")},
            {string_lit("How/./Doing"), string_lit("How/Doing")},
            {string_lit("How/././././Doing"), string_lit("How/Doing")},
            {string_lit("How///Doing"), string_lit("How/Doing")},
            {string_lit("How/You/../Doing/../You/Doing"), string_lit("How/You/Doing")},
            {string_lit("/How/You/../Doing/../You/Doing"), string_lit("/How/You/Doing")},
            {string_lit("c:/How/You/../Doing/../You/Doing"), string_lit("C:/How/You/Doing")},
            {string_lit("Hello/How/.//.//../You"), string_lit("Hello/You")},
            {string_lit("How/../You/../Doing"), string_lit("Doing")},
            {string_lit("How/../..\\../Doing"), string_lit("Doing")},
            {string_lit("../..\\.."), string_lit("")},
            {string_lit("/..\\../.."), string_lit("/")},
            {string_lit("C:\\..\\..\\.."), string_lit("C:/")},
            {string_lit("\\Hello"), string_lit("Hello")},
        };

        DynString string = dynstring_create_over(mem_stack(128));
        for (usize i = 0; i != array_elems(data); ++i) {
            dynstring_clear(&string);
            path_canonize(&string, data[i].path);
            anvil_eq_string(dynstring_view(&string), data[i].expected);
        }
        dynstring_destroy(&string);
    }

    it("can append paths together") {
        DynString string = dynstring_create_over(mem_stack(128));

        path_append(&string, string_lit("Hello"));
        path_append(&string, string_lit("How"));
        path_append(&string, string_lit("You"));
        path_append(&string, string_lit("Doing?"));

        anvil_eq_string(dynstring_view(&string), string_lit("Hello/How/You/Doing?"));

        dynstring_destroy(&string);
    }

    it("can generate a random file-name") {
        static const u64 seed = 42;
        Allocator* alloc = alloc_bump_create_stack(256);
        Rng* rng = rng_create_xorwow(alloc, seed);

        anvil_eq_string(path_name_random_scratch(rng, string_empty), string_lit("nkOZrR4b15bJ"));
        anvil_eq_string(path_name_random_scratch(rng, string_lit("hello")), string_lit("hello_ecfcmkK1mPyR"));
    }

    it("can generate a tiemstampped file-name") {
        const String nameWithoutPrefix = path_name_timestamp_scratch(string_empty);
        anvil_eq_int(nameWithoutPrefix.size, 15);

        const String nameWithPrefix = path_name_timestamp_scratch(string_lit("hello"));
        anvil_eq_int(nameWithPrefix.size, 21);
    }

    it("can retrieve the executable path") {
        anvil(!string_is_empty(g_path_executable));
        anvil(path_is_absolute(g_path_executable));
    }

    it("can retrieve the working-directory path") {
        anvil(!string_is_empty(g_path_workingdir));
        anvil(path_is_absolute(g_path_workingdir));
    }

    it("can retrieve the system temp path") {
        anvil(!string_is_empty(g_path_tempdir));
        anvil(path_is_absolute(g_path_tempdir));
    }
}
