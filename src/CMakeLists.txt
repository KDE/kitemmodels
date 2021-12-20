add_subdirectory(core)
if (TARGET Qt${QT_MAJOR_VERSION}::Qml)
    add_subdirectory(qml)
endif()

ecm_qt_install_logging_categories(
    EXPORT KITEMMODELS
    FILE kitemmodels.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
