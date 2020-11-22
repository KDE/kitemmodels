
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
    descendantqmltree.cpp
)

find_package(Qt5Widgets ${REQUIRED_QT_VERSION} QUIET CONFIG)
find_package(Qt5Script ${REQUIRED_QT_VERSION} QUIET CONFIG)
if (Qt5Script_FOUND)
  list(APPEND proxymodeltestapp_SRCS
    reparentingpmwidget.cpp
    scriptablereparentingwidget.cpp
  )
endif()

find_package(Qt5QuickWidgets ${REQUIRED_QT_VERSION} CONFIG)
if (Qt5QuickWidgets_FOUND AND NOT Qt5QuickWidgets_VERSION VERSION_LESS 5.5)
  list(APPEND proxymodeltestapp_SRCS
    selectioninqmlwidget.cpp
  )
  add_definitions(-DSRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
endif()

add_executable(proxymodeltestapp ${proxymodeltestapp_SRCS})

target_link_libraries(proxymodeltestapp
   KF5::ItemModels
   proxymodeltestsuite
   Qt5::Widgets
)

if (Qt5Script_FOUND)
  target_link_libraries(proxymodeltestapp
    Qt5::Script
  )
endif()
if (Qt5QuickWidgets_FOUND AND NOT Qt5QuickWidgets_VERSION VERSION_LESS 5.5)
  target_link_libraries(proxymodeltestapp
    Qt5::QuickWidgets
  )
endif()
