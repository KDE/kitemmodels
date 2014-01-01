
set(kitemmodels_SRCS
  kbreadcrumbselectionmodel.cpp
  kcheckableproxymodel.cpp
  kdescendantsproxymodel.cpp
  klinkitemselectionmodel.cpp
  kmodelindexproxymapper.cpp
  krecursivefilterproxymodel.cpp
  kselectionproxymodel.cpp
)

add_library(KF5ItemModels ${kitemmodels_SRCS})
generate_export_header(KF5ItemModels BASE_NAME KItemModels)
add_library(KF5::ItemModels ALIAS KF5ItemModels)

target_include_directories(KF5ItemModels INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/kitemmodels>" "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KItemModels>")

target_link_libraries(KF5ItemModels PUBLIC Qt5::Core)

set_target_properties(KF5ItemModels PROPERTIES VERSION   ${KITEMMODELS_VERSION_STRING}
                                               SOVERSION ${KITEMMODELS_SOVERSION}
                                               EXPORT_NAME ItemModels
)

ecm_generate_headers(
  KBreadcrumbSelectionModel
  KCheckableProxyModel
  KLinkItemSelectionModel
  KRecursiveFilterProxyModel
  KDescendantsProxyModel
  KModelIndexProxyMapper
  KSelectionProxyModel

  REQUIRED_HEADERS KItemModels_HEADERS
)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/KItemModels DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

install(TARGETS KF5ItemModels EXPORT KF5ItemModelsTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kitemmodels_export.h
  ${KItemModels_HEADERS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/kitemmodels COMPONENT Devel
)
