include_guard(GLOBAL)

set(CMAKE_C_COMPILER gcc-11)
set(CMAKE_CXX_COMPILER g++-11)

set(CMAKE_CXX_FLAGS
  "-std=c++20 \
   -fcoroutines \
   -Wall -Wextra "
CACHE STRING "CXX_FLAGS" FORCE)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3" CACHE STRING "C++ DEBUG Flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -DNDEBUG" CACHE STRING "C++ Release Flags" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -DNDEBUG" CACHE STRING "C++ RelWithDebInfo Flags" FORCE)
