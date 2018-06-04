set(corebindings_SRCS
    plugin.cpp
    kconcatenaterowsproxymodel_qml.cpp
    knumbermodel.cpp
    #ProxyModel
)

add_library(itemmodelsplugin SHARED ${corebindings_SRCS})
target_link_libraries(itemmodelsplugin
        Qt5::Qml
        KF5::ItemModels
)

install(TARGETS itemmodelsplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kitemmodels)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kitemmodels)

# TODO what's this?
# ecm_generate_qmltypes(org.kde.plasma.core 2.0 DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/core)
