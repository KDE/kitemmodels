remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

add_subdirectory(proxymodeltestapp)
macro(KITEMMODELS_WIDGETS_TESTS)
  foreach(_testname ${ARGN})
    add_executable(${_testname} ${_testname}.cpp)
    target_link_libraries(${_testname}
        KF5::ItemModels
        Qt${QT_MAJOR_VERSION}::Widgets)
    ecm_mark_as_test(${_testname})
  endforeach()
endmacro(KITEMMODELS_WIDGETS_TESTS)

KITEMMODELS_WIDGETS_TESTS(
  rearrangecolumns
)
