add_library(KF5ItemModels)
add_library(KF5::ItemModels ALIAS KF5ItemModels)

set_target_properties(KF5ItemModels PROPERTIES
    VERSION     ${KITEMMODELS_VERSION}
    SOVERSION   ${KITEMMODELS_SOVERSION}
    EXPORT_NAME ItemModels
)

target_sources(KF5ItemModels PRIVATE
  kbreadcrumbselectionmodel.cpp
  kcheckableproxymodel.cpp
  kconcatenaterowsproxymodel.cpp
  kdescendantsproxymodel.cpp
  kextracolumnsproxymodel.cpp
  klinkitemselectionmodel.cpp
  kmodelindexproxymapper.cpp
  knumbermodel.cpp
  krearrangecolumnsproxymodel.cpp
  krecursivefilterproxymodel.cpp
  kselectionproxymodel.cpp
  kcolumnheadersmodel.cpp
)

ecm_qt_declare_logging_category(KF5ItemModels
    HEADER kitemmodels_debug.h
    IDENTIFIER KITEMMODELS_LOG
    CATEGORY_NAME kf.itemmodels.core
    OLD_CATEGORY_NAMES kf5.kitemmodels
    DESCRIPTION "KItemModels (Core)"
    EXPORT KITEMMODELS
)

ecm_generate_export_header(KF5ItemModels
    BASE_NAME KItemModels
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 4.8 5.65 5.80
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

target_include_directories(KF5ItemModels INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KItemModels>")

target_link_libraries(KF5ItemModels PUBLIC Qt5::Core)

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
  KNumberModel
  KColumnHeadersModel
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
      kcolumnheadersmodel.h
  )
endif()

install(TARGETS KF5ItemModels EXPORT KF5ItemModelsTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kitemmodels_export.h
  ${KItemModels_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KItemModels COMPONENT Devel
)

if(BUILD_QCH)
    ecm_add_qch(
        KF5ItemModels_QCH
        NAME KItemModels
        BASE_NAME KF5ItemModels
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KItemModels_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        IMAGE_DIRS "${CMAKE_SOURCE_DIR}/docs/pics"
        LINK_QCHS
            Qt5Core_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            KITEMMODELS_EXPORT
            "KITEMMODELS_DEPRECATED_VERSION(x, y, t)"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KItemModels LIB_NAME KF5ItemModels DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KItemModels)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
