
set(proxyModelSmokeTestSources
  kselectionproxymodeltestsuite.cpp
)


remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test Widgets)

add_subdirectory(proxymodeltestsuite)

include(ECMAddTests)

ecm_add_tests(
  kconcatenaterowsproxymodeltest.cpp
  kdescendantsproxymodel_smoketest.cpp
  kdescendantsproxymodeltest.cpp
  kextracolumnsproxymodeltest.cpp
  klinkitemselectionmodeltest.cpp
  testmodelqueuedconnections.cpp
  kselectionproxymodeltest.cpp
  kmodelindexproxymappertest.cpp
  krecursivefilterproxymodeltest.cpp
  krearrangecolumnsproxymodeltest.cpp
  LINK_LIBRARIES KF5::ItemModels Qt5::Test Qt5::Widgets proxymodeltestsuite
)

#we need additional sources for this test, can't use it in ecm_add_tests
ecm_add_test(kselectionproxymodel_smoketest.cpp ${proxyModelSmokeTestSources}
  TEST_NAME "kselectionproxymodel_smoketest"
  LINK_LIBRARIES KF5::ItemModels Qt5::Test Qt5::Widgets proxymodeltestsuite
)

if (PythonModuleGeneration_FOUND AND EXISTS ${CMAKE_BINARY_DIR}/prefix/lib/python2.7/dist-packages)
  add_test(NAME Py2${name} COMMAND python2 ${pythontest} ${CMAKE_BINARY_DIR}/prefix/lib/python2.7/dist-packages)
endif()

if (PythonModuleGeneration_FOUND AND EXISTS ${CMAKE_BINARY_DIR}/prefix/lib/python3.5/dist-packages)
  add_test(NAME Py3${name} COMMAND python3 ${pythontest} ${CMAKE_BINARY_DIR}/prefix/lib/python3.5/dist-packages)
endif()
