#pragma once

#define IMPL_GET_10TH_ARG(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _N_, ...) _N_

#define COUNT_VA_ARGS(...) \
    IMPL_GET_10TH_ARG("ignored", ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define VA_ARGS_SKIP_FIRST(_FIRST_, ...) __VA_ARGS__