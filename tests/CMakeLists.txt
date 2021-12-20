remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

if (TARGET Qt${QT_MAJOR_VERSION}::QuickWidgets)
    add_subdirectory(proxymodeltestapp)
endif()
find_package(Qt${QT_MAJOR_VERSION}Widgets ${REQUIRED_QT_VERSION} CONFIG)
if (TARGET Qt${QT_MAJOR_VERSION}::Widgets)
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
endif()
