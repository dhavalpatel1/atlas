#pragma once

#include "core_dynstring.h"

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
