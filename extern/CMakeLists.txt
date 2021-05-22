add_subdirectory(googletest EXCLUDE_FROM_ALL)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
add_subdirectory(benchmark EXCLUDE_FROM_ALL)
