
set(proxymodeltestapp_SRCS
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
  list(APPEND proxymodeltestapp_SRCS
    reparentingpmwidget.cpp
    scriptablereparentingwidget.cpp
  )
endif()

if (Qt${QT_MAJOR_VERSION}QuickWidgets_FOUND)
  list(APPEND proxymodeltestapp_SRCS
    selectioninqmlwidget.cpp
    descendantqmltree.cpp
  )
  add_definitions(-DSRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
endif()

add_executable(proxymodeltestapp ${proxymodeltestapp_SRCS})

target_link_libraries(proxymodeltestapp
   KF5::ItemModels
   proxymodeltestsuite
   Qt${QT_MAJOR_VERSION}::Widgets
)

if (TARGET Qt5::Script)
  target_link_libraries(proxymodeltestapp
    Qt${QT_MAJOR_VERSION}::Script
  )
endif()
if (TARGET Qt${QT_MAJOR_VERSION}::QuickWidgets)
  target_link_libraries(proxymodeltestapp
    Qt${QT_MAJOR_VERSION}::QuickWidgets
  )
endif()
