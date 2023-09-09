ecm_add_qml_module(itemmodelsplugin URI "org.kde.kitemmodels" CLASSNAME Plugin)

target_sources(itemmodelsplugin PRIVATE
    kdescendantsproxymodel_qml.cpp
    kdescendantsproxymodel_qml.h
    krolenames.cpp
    krolenames.h
    ksortfilterproxymodel.cpp
    ksortfilterproxymodel.h
    plugin.cpp
    plugin.h
)

ecm_qt_declare_logging_category(itemmodelsplugin
    HEADER kitemmodels_debug.h
    IDENTIFIER KITEMMODELS_LOG
    CATEGORY_NAME kf.itemmodels.quick
    DESCRIPTION "KItemModels (QtQuick)"
    EXPORT KITEMMODELS
)

target_link_libraries(itemmodelsplugin PRIVATE
    Qt6::Qml
    KF6::ItemModels
)

ecm_finalize_qml_module(itemmodelsplugin DESTINATION ${KDE_INSTALL_QMLDIR})
