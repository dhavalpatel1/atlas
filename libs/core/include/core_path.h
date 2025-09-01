#pragma once

#include "core_dynstring.h"
#include "core_macro.h"

typedef struct sRng Rng;

#define path_build(_DYNSTRING_, ...) path_build_raw((_DYNSTRING_), (const String[]){ VA_ARGS_SKIP_FIRST(0, ##__VA_ARGS__, string_empty) })

#define path_build_scratch(...) path_build_scratch_raw((const String[]){ VA_ARGS_SKIP_FIRST(0, ##__VA_ARGS__, string_empty) })

extern String g_path_workingdir;

extern String g_path_executable;

extern String g_path_tempdir;

bool path_is_absolute(String path);

bool path_is_root(String path);

String path_filename(String path);

String path_extension(String path);

String path_stem(String path);

String path_parent(String path);

bool path_canonize(DynString* result, String path);

void path_append(DynString* basePath, String path);

void path_build_raw(DynString* str, const String* segments);

String path_build_scratch_raw(const String* segments);

void path_name_random(DynString*, Rng*, String prefix, String extension);

String path_name_random_scratch(Rng*, String prefix, String extension);

void path_name_timestamp(DynString*, String prefix, String extension);

String path_name_timestamp_scratch(String prefix, String extension);
