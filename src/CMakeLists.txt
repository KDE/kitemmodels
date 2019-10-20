
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

ecm_qt_declare_logging_category(kitemmodels_SRCS HEADER kitemmodels_debug.h IDENTIFIER KITEMMODELS_LOG CATEGORY_NAME kf5.kitemmodels)

add_library(KF5ItemModels ${kitemmodels_SRCS})
add_library(KF5::ItemModels ALIAS KF5ItemModels)
ecm_generate_export_header(KF5ItemModels
    BASE_NAME KItemModels
    GROUP_BASE_NAME KF
    VERSION ${KF5_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 4.8
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)
# reminder trigger to apply KITEMMODELS_ENABLE_DEPRECATED_SINCE & friends to KConcatenateRowsProxyModel when the time is there
if (REQUIRED_QT_VERSION VERSION_GREATER 5.12.0)
    message(WARNING "With Qt 5.13 as min dep, mark KConcatenateRowsProxyModel as deprecated to the compiler & remove this warning")
endif()


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

if(BUILD_QCH)
    ecm_add_qch(
        KF5ItemModels_QCH
        NAME KItemModels
        BASE_NAME KF5ItemModels
        VERSION ${KF5_VERSION}
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
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KItemModels LIB_NAME KF5ItemModels DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KItemModels)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
