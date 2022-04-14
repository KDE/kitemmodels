add_executable(proxymodeltestapp)

target_sources(proxymodeltestapp PRIVATE
    main.cpp
    mainwindow.cpp
    breadcrumbswidget.cpp
    breadcrumbnavigationwidget.cpp
    breadcrumbdirectionwidget.cpp
    checkablewidget.cpp
    selectionpmwidget.cpp
#     statesaverwidget.cpp
    kidentityproxymodelwidget.cpp
    descendantpmwidget.cpp
    kreparentingproxymodel.cpp
    proxymodeltestwidget.cpp
    proxyitemselectionwidget.cpp
    lessthanwidget.cpp
    modelcommanderwidget.cpp
    matchcheckingwidget.cpp
)

if (TARGET Qt5::Script)
  target_sources(proxymodeltestapp PRIVATE
    reparentingpmwidget.cpp
    scriptablereparentingwidget.cpp
  )
  target_link_libraries(proxymodeltestapp Qt${QT_MAJOR_VERSION}::Script)
endif()

if (TARGET Qt${QT_MAJOR_VERSION}::QuickWidgets)
  target_sources(proxymodeltestapp PRIVATE
    selectioninqmlwidget.cpp
    descendantqmltree.cpp
  )
  target_link_libraries(proxymodeltestapp Qt${QT_MAJOR_VERSION}::QuickWidgets)
  target_compile_definitions(proxymodeltestapp PRIVATE -DSRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
endif()

target_link_libraries(proxymodeltestapp
   KF5::ItemModels
   proxymodeltestsuite
   Qt${QT_MAJOR_VERSION}::Widgets
)
