cmake_minimum_required(VERSION 2.8.12)

project(KItemModels)

find_package(ECM 0.0.13 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION 5.2.0)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)

include(FeatureSummary)
include(GenerateExportHeader)

include(ECMSetupVersion)
include(ECMGenerateHeaders)

set(KF5_VERSION "4.99.0") # handled by release scripts

ecm_setup_version(${KF5_VERSION} VARIABLE_PREFIX KITEMMODELS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kitemmodels_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5ItemModelsConfigVersion.cmake"
                        SOVERSION 5)


add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()


# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5ItemModels")

include(ECMPackageConfigHelpers)

ecm_configure_package_config_file(
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
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR} COMPONENT Devel
)

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
