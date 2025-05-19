#pragma once

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

#define ASSERTMSG(exp, msg) assert((void(msg), exp))

