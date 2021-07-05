include_guard( GLOBAL )

# Debug
set(CMAKE_C_FLAGS_DEBUG
  "-O0 -fno-inline -g3"
  CACHE STRING
  "C DEBUG Flags"
  FORCE)

set(CMAKE_CXX_FLAGS_DEBUG
  "-O0 -fno-inline -g3"
  CACHE STRING
  "C++ DEBUG Flags"
  FORCE)

#Release
set(CMAKE_C_FLAGS_RELEASE
  "-Ofast -g0 -DNDEBUG"
  CACHE STRING "C Release Flags"
  FORCE)

set(CMAKE_CXX_FLAGS_RELEASE
  "-Ofast -g0 -DNDEBUG"
  CACHE STRING "C++ Release Flags"
  FORCE)

#RelWithdebinfo
set(CMAKE_C_FLAGS_RELWITHDEBINFO
  "-O3 -g -DNDEBUG"
  CACHE STRING "C++ RelWithDebInfo Flags"
  FORCE)

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
  "-O3 -g -DNDEBUG"
  CACHE STRING "C++ RelWithDebInfo Flags"
  FORCE)

# ThreadSanitizer
set(CMAKE_C_FLAGS_TSAN
    "-fsanitize=thread -g -O3 -DNDEBUG"
    CACHE STRING "Flags used by the C compiler during ThreadSanitizer builds."
    FORCE)

set(CMAKE_CXX_FLAGS_TSAN
    "-fsanitize=thread -g -O3 -DNDEBUG"
    CACHE STRING "Flags used by the C++ compiler during ThreadSanitizer builds."
    FORCE)

# AddressSanitize
set(CMAKE_C_FLAGS_ASAN
    "-fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O3 -DNDEBUG"
    CACHE STRING "Flags used by the C compiler during AddressSanitizer builds."
    FORCE)

set(CMAKE_CXX_FLAGS_ASAN
    "-fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O3 -DNDEBUG"
    CACHE STRING "Flags used by the C++ compiler during AddressSanitizer builds."
    FORCE)

# LeakSanitizer
set(CMAKE_C_FLAGS_LSAN
    "-fsanitize=leak -fno-omit-frame-pointer -g -O3 -DNDEBUG"
    CACHE STRING "Flags used by the C compiler during LeakSanitizer builds."
    FORCE)
set(CMAKE_CXX_FLAGS_LSAN
    "-fsanitize=leak -fno-omit-frame-pointer -g -O3 -DNDEBUG"
    CACHE STRING "Flags used by the C++ compiler during LeakSanitizer builds."
    FORCE)


# UndefinedBehaviour
set(CMAKE_C_FLAGS_UBSAN
    "-fsanitize=undefined -g -O3 -DNDEBUG"
    CACHE STRING "Flags used by the C compiler during UndefinedBehaviourSanitizer builds."
    FORCE)
set(CMAKE_CXX_FLAGS_UBSAN
    "-fsanitize=undefined -g -O3 -DNDEBUG"
    CACHE STRING "Flags used by the C++ compiler during UndefinedBehaviourSanitizer builds."
    FORCE)
