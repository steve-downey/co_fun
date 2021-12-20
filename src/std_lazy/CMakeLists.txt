add_library(std_lazy STATIC "")

target_sources(
  std_lazy
  PRIVATE
  std_lazy.cpp)

include(GNUInstallDirs)

target_include_directories(std_lazy PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}>  # <prefix>/include/co_fun
  )

install(
  TARGETS std_lazy
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

string(TOLOWER ${CMAKE_PROJECT_NAME} CMAKE_LOWER_PROJECT_NAME)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h"
  )


## Tests
add_executable(std_lazy_test "")

target_sources(
  std_lazy_test
  PRIVATE
  std_lazy.t.cpp)

target_link_libraries(std_lazy_test std_lazy)
target_link_libraries(std_lazy_test gtest)
target_link_libraries(std_lazy_test gtest_main)

include(GoogleTest)
gtest_discover_tests(std_lazy_test)

# add_executable(
#   co_fun_benchmark
#   stream.b.cpp
#   )

# target_link_libraries(co_fun_benchmark benchmark delay)
