
set(proxyModelSmokeTestSources
  kselectionproxymodeltestsuite.cpp
)


remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test Widgets)

add_subdirectory(proxymodeltestsuite)
add_subdirectory(bihash)

include(ECMAddTests)

if(NOT EXCLUDE_DEPRECATED_BEFORE_AND_AT STREQUAL "CURRENT" AND
   EXCLUDE_DEPRECATED_BEFORE_AND_AT VERSION_LESS 5.65.0)
    set(krecursivefilterproxymodeltest_SRC krecursivefilterproxymodeltest.cpp)
endif()

if(NOT EXCLUDE_DEPRECATED_BEFORE_AND_AT STREQUAL "CURRENT" AND
   EXCLUDE_DEPRECATED_BEFORE_AND_AT VERSION_LESS 5.80.0)
    set(kconcatenaterowsproxymodeltest_SRC kconcatenaterowsproxymodeltest.cpp)
    set(kconcatenaterows_qml_SRC kconcatenaterows_qml.cpp)
endif()

ecm_add_tests(
  ${kconcatenaterowsproxymodeltest_SRC}
  kdescendantsproxymodel_smoketest.cpp
  kdescendantsproxymodeltest.cpp
  kextracolumnsproxymodeltest.cpp
  klinkitemselectionmodeltest.cpp
  kselectionproxymodeltest.cpp
  kmodelindexproxymappertest.cpp
  ${krecursivefilterproxymodeltest_SRC}
  krearrangecolumnsproxymodeltest.cpp
  knumbermodeltest.cpp
  kcolumnheadersmodeltest.cpp
  LINK_LIBRARIES KF5::ItemModels Qt5::Test Qt5::Widgets proxymodeltestsuite
)

if (${Qt5Qml_FOUND})
    ecm_add_tests(
        ${kconcatenaterows_qml_SRC}
        ksortfilterproxymodel_qml.cpp
        LINK_LIBRARIES KF5::ItemModels Qt5::Test Qt5::Qml Qt5::Gui
    )
endif()

#we need additional sources for this test, can't use it in ecm_add_tests
ecm_add_test(kselectionproxymodel_smoketest.cpp ${proxyModelSmokeTestSources}
  TEST_NAME "kselectionproxymodel_smoketest"
  LINK_LIBRARIES KF5::ItemModels Qt5::Test Qt5::Widgets proxymodeltestsuite
)
