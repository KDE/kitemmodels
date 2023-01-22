
set(proxyModelSmokeTestSources
  kselectionproxymodeltestsuite.cpp
)


remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)

add_subdirectory(proxymodeltestsuite)
add_subdirectory(bihash)

include(ECMAddTests)

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
  LINK_LIBRARIES KF6::ItemModels Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Widgets proxymodeltestsuite
)

if (TARGET Qt${QT_MAJOR_VERSION}::Qml)
    ecm_add_tests(
        ${kconcatenaterows_qml_SRC}
        ksortfilterproxymodel_qml.cpp
        LINK_LIBRARIES KF6::ItemModels Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Qml Qt${QT_MAJOR_VERSION}::Gui
    )

    # In case of static builds, we need to link against and import the QML plugin 
    if (NOT BUILD_SHARED_LIBS)
      target_link_libraries(ksortfilterproxymodel_qml itemmodelsplugin)
      target_compile_definitions(ksortfilterproxymodel_qml PRIVATE -DIMPORT_ITEMMODELSPLUGIN)
    endif()
endif()

#we need additional sources for this test, can't use it in ecm_add_tests
ecm_add_test(kselectionproxymodel_smoketest.cpp ${proxyModelSmokeTestSources}
  TEST_NAME "kselectionproxymodel_smoketest"
  LINK_LIBRARIES KF6::ItemModels Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Widgets proxymodeltestsuite
)
