
set(kitemmodels_SRCS
  kbreadcrumbselectionmodel.cpp
  kcheckableproxymodel.cpp
  kconcatenaterowsproxymodel.cpp
  kdescendantsproxymodel.cpp
  kextracolumnsproxymodel.cpp
  klinkitemselectionmodel.cpp
  kmodelindexproxymapper.cpp
  krearrangecolumnsproxymodel.cpp
  krecursivefilterproxymodel.cpp
  kselectionproxymodel.cpp
)

add_library(KF5ItemModels ${kitemmodels_SRCS})
generate_export_header(KF5ItemModels BASE_NAME KItemModels)
add_library(KF5::ItemModels ALIAS KF5ItemModels)

target_include_directories(KF5ItemModels INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KItemModels>")

target_link_libraries(KF5ItemModels PUBLIC Qt5::Core)

set_target_properties(KF5ItemModels PROPERTIES VERSION   ${KITEMMODELS_VERSION_STRING}
                                               SOVERSION ${KITEMMODELS_SOVERSION}
                                               EXPORT_NAME ItemModels
)

ecm_generate_headers(KItemModels_HEADERS
  HEADER_NAMES
  KBreadcrumbSelectionModel
  KConcatenateRowsProxyModel
  KCheckableProxyModel
  KExtraColumnsProxyModel
  KLinkItemSelectionModel
  KRearrangeColumnsProxyModel
  KRecursiveFilterProxyModel
  KDescendantsProxyModel
  KModelIndexProxyMapper
  KSelectionProxyModel

  REQUIRED_HEADERS KItemModels_HEADERS
)

find_package(PythonModuleGeneration)

if (PythonModuleGeneration_FOUND)
  ecm_generate_python_binding(
    TARGET KF5::ItemModels
    PYTHONNAMESPACE PyKF5
    MODULENAME KItemModels
    SIP_DEPENDS
      QtCore/QtCoremod.sip
    HEADERS
      kbreadcrumbselectionmodel.h
      kconcatenaterowsproxymodel.h
      kcheckableproxymodel.h
      kextracolumnsproxymodel.h
      klinkitemselectionmodel.h
      krearrangecolumnsproxymodel.h
      krecursivefilterproxymodel.h
      kdescendantsproxymodel.h
      kmodelindexproxymapper.h
      kselectionproxymodel.h
  )
endif()

install(TARGETS KF5ItemModels EXPORT KF5ItemModelsTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kitemmodels_export.h
  ${KItemModels_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KItemModels COMPONENT Devel
)

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KItemModels LIB_NAME KF5ItemModels DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KItemModels)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
