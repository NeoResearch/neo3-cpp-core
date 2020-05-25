#pragma once

#include <neo3-cpp-core/neopt-common/system/types.h>

// perform sha256 on input and returns used size (in bytes). output must be pre-allocated.
//extern "C" int
extern "C" int
external_sha256(neopt::byte* input, int sz_input, neopt::byte* output, int sz_output);

// perform ripemd160 on input and returns used size (in bytes). output must be pre-allocated.
//extern "C" int
extern "C" int
external_ripemd160(neopt::byte* input, int sz_input, neopt::byte* output, int sz_output);