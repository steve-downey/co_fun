add_library(delay STATIC "")

target_sources(
  delay
  PRIVATE
  delay.cpp)

include(GNUInstallDirs)

target_include_directories(delay PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}>  # <prefix>/include/delay
  )

install(
  TARGETS delay
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
add_executable(delay_test "")

target_sources(
  delay_test
  PRIVATE
  delay.t.cpp
  delayasync.t.cpp
  stream.t.cpp
  streamasync.t.cpp)

target_link_libraries(delay_test delay)
target_link_libraries(delay_test gtest)
target_link_libraries(delay_test gtest_main)

include(GoogleTest)
gtest_discover_tests(delay_test)

add_executable(
  delay_benchmark
  stream.b.cpp
  streamasync.b.cpp
  )

target_link_libraries(delay_benchmark benchmark delay)
