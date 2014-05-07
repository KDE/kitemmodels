
set(proxyModelTestSources
  kselectionproxymodeltestsuite.cpp
)


remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)

find_package(Qt5 5.2.0 CONFIG REQUIRED Test Widgets)

add_subdirectory(proxymodeltestsuite)

include(ECMAddTests)

ecm_add_tests(
  kdescendantsproxymodeltest.cpp
  klinkitemselectionmodeltest.cpp
  testmodelqueuedconnections.cpp
  NAME_PREFIX "kitemmodels-"
  LINK_LIBRARIES KF5::ItemModels Qt5::Test Qt5::Widgets proxymodeltestsuite
)

#we need additional sources for this test, can't use it in ecm_add_tests
ecm_add_test(kselectionproxymodeltest.cpp ${proxyModelTestSources}
  TEST_NAME "kitemmodels-kselectionproxymodeltest"
  LINK_LIBRARIES KF5::ItemModels Qt5::Test Qt5::Widgets proxymodeltestsuite
)

if (Qt5Script_FOUND)
    add_subdirectory(proxymodeltestapp)
endif()
