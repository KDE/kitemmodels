add_library(proxymodeltestsuite)
add_library(KF6::proxymodeltestsuite ALIAS proxymodeltestsuite)

target_sources(proxymodeltestsuite PRIVATE
    dynamictreemodel.cpp
    modelcommander.cpp
    modeldumper.cpp
    modelselector.cpp
    modelspy.cpp
    modeltest.cpp
    proxymodeltest.cpp

    eventlogger.qrc
)

generate_export_header(proxymodeltestsuite)

target_link_libraries(proxymodeltestsuite
    Qt6::Test
    Qt6::Gui
)
