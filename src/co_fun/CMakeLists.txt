add_library(co_fun STATIC "")

target_sources(
  co_fun
  PRIVATE
  co_fun.cpp)

include(GNUInstallDirs)

target_include_directories(co_fun PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}>  # <prefix>/include/co_fun
  )

install(
  TARGETS co_fun
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
add_executable(co_fun_test "")

target_sources(
  co_fun_test
  PRIVATE
  co_fun.t.cpp)

target_link_libraries(co_fun_test co_fun)
target_link_libraries(co_fun_test gtest)
target_link_libraries(co_fun_test gtest_main)

include(GoogleTest)
gtest_discover_tests(co_fun_test)
