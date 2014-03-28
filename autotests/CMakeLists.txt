
set(proxyModelTestSources
  kselectionproxymodeltestsuite.cpp
)


remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)

find_package(Qt5 5.2.0 CONFIG REQUIRED Test Widgets)

add_subdirectory(proxymodeltestsuite)

include(ECMMarkAsTest)

macro(kitemmodels_proxymodel_tests)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp ${proxyModelTestSources} )
        add_test(kitemmodels-${_testname} ${_testname})
        ecm_mark_as_test(${_testname})
        target_link_libraries(${_testname} KF5::ItemModels Qt5::Test Qt5::Widgets proxymodeltestsuite )
    endforeach(_testname)
endmacro()


kitemmodels_proxymodel_tests(
  kdescendantsproxymodeltest
  klinkitemselectionmodeltest
  # TODO fix this test - or the code that it tests!
  # FAIL!  : SelectionProxyModelTest::testMoveFromRoot(move01) 'signalList.isEmpty()' returned FALSE. ()
  #  Loc: [/srv/jenkins/workspace/kitemmodels_master_qt5/autotests/proxymodeltestsuite/proxymodeltest.cpp(569)]
  #  QFATAL : SelectionProxyModelTest::testMoveFromRoot(move02) ASSERT: "m_unchangedIndexes.isEmpty()" in file proxymodeltestsuite/modelspy.cpp, line 253
  #kselectionproxymodeltest
  testmodelqueuedconnections
)

if (Qt5Script_FOUND)
    add_subdirectory(proxymodeltestapp)
endif()
