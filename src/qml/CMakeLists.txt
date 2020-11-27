set(corebindings_SRCS
    plugin.cpp
    kconcatenaterowsproxymodel_qml.cpp
    kdescendantsproxymodel_qml.cpp
    ksortfilterproxymodel.cpp
)

ecm_qt_declare_logging_category(corebindings_SRCS
    HEADER kitemmodels_debug.h
    IDENTIFIER KITEMMODELS_LOG
    CATEGORY_NAME kf.itemmodels.quick
    DESCRIPTION "KItemModels (QtQuick)"
    EXPORT KITEMMODELS
)

ecm_qt_declare_logging_category(corebindings_SRCS
    HEADER kitemmodelsqml_logdeprecated.h
    IDENTIFIER KITEMMODELS_LOGDEPRECATED
    CATEGORY_NAME kf.itemmodels.quick.deprecations
    DEFAULT_SEVERITY Warning
    DESCRIPTION "KItemModels (QtQuick deprecation warnings)"
    EXPORT KITEMMODELS
)

add_library(itemmodelsplugin SHARED ${corebindings_SRCS})
target_link_libraries(itemmodelsplugin
        Qt5::Qml
        KF5::ItemModels
)

install(TARGETS itemmodelsplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kitemmodels)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kitemmodels)

# Faux install into the build dir for unit tests

if(BUILD_TESTING)
    add_custom_target(copy ALL DEPENDS itemmodelsplugin)
    add_custom_command(TARGET copy POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E
                            copy ${CMAKE_CURRENT_SOURCE_DIR}/qmldir ${CMAKE_BINARY_DIR}/bin/org/kde/kitemmodels/qmldir)
    add_custom_command(TARGET copy POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E
                        copy $<TARGET_FILE:itemmodelsplugin> ${CMAKE_BINARY_DIR}/bin/org/kde/kitemmodels)
endif()
