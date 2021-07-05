include_guard(GLOBAL)

set(CMAKE_C_COMPILER gcc-11)
set(CMAKE_CXX_COMPILER g++-11)

set(CMAKE_CXX_FLAGS
  "-std=c++20 \
   -fcoroutines \
   -fconcepts-diagnostics-depth=3 \
   -Wall -Wextra "
  CACHE STRING "CXX_FLAGS"
  FORCE)

include("${CMAKE_CURRENT_LIST_DIR}/common-flags.cmake")
