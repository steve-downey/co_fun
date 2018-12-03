add_library(scratch STATIC "")

target_sources(
  scratch
  PRIVATE
  scratch.cpp)

include(GNUInstallDirs)

target_include_directories(scratch PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}>  # <prefix>/include/scratch
  )

install(
  TARGETS scratch
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
add_executable(scratch_test "")

target_sources(
  scratch_test
  PRIVATE
  scratch.t.cpp)

target_link_libraries(scratch_test scratch)
target_link_libraries(scratch_test gtest)
target_link_libraries(scratch_test gtest_main)

include(GoogleTest)
gtest_discover_tests(scratch_test)
