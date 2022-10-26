ecm_add_qml_module(itemmodelsplugin URI "org.kde.kitemmodels" CLASSNAME Plugin)

target_sources(itemmodelsplugin PRIVATE
    kdescendantsproxymodel_qml.cpp
    kdescendantsproxymodel_qml.h
    ksortfilterproxymodel.cpp
    ksortfilterproxymodel.h
    plugin.cpp
    plugin.h
    qmldeprecated.h
)
if (QT_MAJOR_VERSION EQUAL "5")
    target_sources(itemmodelsplugin PRIVATE
        kconcatenaterowsproxymodel_qml.cpp
        kconcatenaterowsproxymodel_qml.h
    )
endif()

ecm_qt_declare_logging_category(itemmodelsplugin
    HEADER kitemmodels_debug.h
    IDENTIFIER KITEMMODELS_LOG
    CATEGORY_NAME kf.itemmodels.quick
    DESCRIPTION "KItemModels (QtQuick)"
    EXPORT KITEMMODELS
)

ecm_qt_declare_logging_category(itemmodelsplugin
    HEADER kitemmodelsqml_logdeprecated.h
    IDENTIFIER KITEMMODELS_LOGDEPRECATED
    CATEGORY_NAME kf.itemmodels.quick.deprecations
    DEFAULT_SEVERITY Warning
    DESCRIPTION "KItemModels (QtQuick deprecation warnings)"
    EXPORT KITEMMODELS
)

target_link_libraries(itemmodelsplugin
        Qt${QT_MAJOR_VERSION}::Qml
        KF5::ItemModels
)

# Faux install into the build dir for unit tests

if(BUILD_TESTING)
    add_custom_target(copy ALL DEPENDS itemmodelsplugin)
    add_custom_command(TARGET copy POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E
                        copy $<TARGET_FILE:itemmodelsplugin> ${CMAKE_BINARY_DIR}/bin/org/kde/kitemmodels)
endif()

ecm_finalize_qml_module(itemmodelsplugin DESTINATION ${KDE_INSTALL_QMLDIR})