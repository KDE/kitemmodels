remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

add_subdirectory(proxymodeltestapp)

find_package(Qt5Widgets ${REQUIRED_QT_VERSION} CONFIG)
if (Qt5Widgets_FOUND)
  macro(KITEMMODELS_WIDGETS_TESTS)
    foreach(_testname ${ARGN})
      add_executable(${_testname} ${_testname}.cpp)
      target_link_libraries(${_testname}
         KF5::ItemModels
         Qt5::Widgets)
      ecm_mark_as_test(${_testname})
    endforeach()
  endmacro(KITEMMODELS_WIDGETS_TESTS)

  KITEMMODELS_WIDGETS_TESTS(
    rearrangecolumns
  )
endif()
