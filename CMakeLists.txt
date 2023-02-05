cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "5.103.0") # handled by release scripts
project(KItemModels VERSION ${KF_VERSION})

include(FeatureSummary)
find_package(ECM 5.103.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(KDEGitCommitHooks)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMGenerateExportHeader)
include(ECMDeprecationSettings)
include(ECMAddQch)
include(CMakePackageConfigHelpers)
include(ECMQmlModule)

set(REQUIRED_QT_VERSION 5.15.2)
find_package(Qt${QT_MAJOR_VERSION}Core ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)
find_package(Qt${QT_MAJOR_VERSION}Qml ${REQUIRED_QT_VERSION} NO_MODULE)

set_package_properties(Qt${QT_MAJOR_VERSION}Qml PROPERTIES
        TYPE OPTIONAL
        PURPOSE "Build QML import for KItemModels"
)


set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

ecm_setup_version(PROJECT VARIABLE_PREFIX KITEMMODELS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kitemmodels_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5ItemModelsConfigVersion.cmake"
                        SOVERSION 5)

ecm_set_disabled_deprecation_versions(
    QT 5.15.2
)

add_subdirectory(src)
if (BUILD_TESTING)
    find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test Widgets)
    find_package(Qt${QT_MAJOR_VERSION}QuickWidgets ${REQUIRED_QT_VERSION} NO_MODULE)
    set_package_properties(Qt${QT_MAJOR_VERSION}QuickWidgets PROPERTIES
        TYPE OPTIONAL
        PURPOSE "Build proxymodeltestapp QML selection and QML treeview examples"
    )
    set_package_properties(Qt${QT_MAJOR_VERSION}Script PROPERTIES
        TYPE OPTIONAL
        PURPOSE "Build proxymodeltestapp reparenting example"
    )
    find_package(Qt${QT_MAJOR_VERSION}Script ${REQUIRED_QT_VERSION} NO_MODULE)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()


# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5ItemModels")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5ItemModels_QCH
        FILE KF5ItemModelsQCHTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5ItemModelsQCHTargets.cmake\")")
endif()

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5ItemModelsConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5ItemModelsConfig.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/KF5ItemModelsConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5ItemModelsConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT KF5ItemModelsTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5ItemModelsTargets.cmake NAMESPACE KF5:: )

install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/kitemmodels_version.h
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KItemModels COMPONENT Devel
)

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
