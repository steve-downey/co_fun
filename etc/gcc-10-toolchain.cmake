include_guard(GLOBAL)

set(CMAKE_C_COMPILER gcc-10)
set(CMAKE_CXX_COMPILER g++-10)

set(CMAKE_CXX_FLAGS
  "-std=c++20 \
   -fcoroutines \
   -Wall -Wextra "
CACHE STRING "CXX_FLAGS" FORCE)

include("${CMAKE_CURRENT_LIST_DIR}/common-flags.cmake")
