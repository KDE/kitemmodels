cmake_minimum_required(VERSION 3.5)

set(KF5_VERSION "5.59.0") # handled by release scripts
project(KItemModels VERSION ${KF5_VERSION})

include(FeatureSummary)
find_package(ECM 5.59.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMQtDeclareLoggingCategory)

set(REQUIRED_QT_VERSION 5.10.0)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)


include(GenerateExportHeader)

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMAddQch)

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

ecm_setup_version(PROJECT VARIABLE_PREFIX KITEMMODELS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kitemmodels_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5ItemModelsConfigVersion.cmake"
                        SOVERSION 5)
add_definitions(-DQT_NO_FOREACH)
add_subdirectory(src)
if (BUILD_TESTING)
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

include(CMakePackageConfigHelpers)

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
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
)

# contains list of debug categories, for kdebugsettings
install(FILES kitemmodels.categories DESTINATION ${KDE_INSTALL_CONFDIR})

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
