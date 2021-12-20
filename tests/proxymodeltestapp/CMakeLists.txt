
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

find_package(Qt${QT_MAJOR_VERSION}Widgets ${REQUIRED_QT_VERSION} QUIET CONFIG)
find_package(Qt${QT_MAJOR_VERSION}Script ${REQUIRED_QT_VERSION} QUIET CONFIG)
if (Qt${QT_MAJOR_VERSION}Script_FOUND)
  list(APPEND proxymodeltestapp_SRCS
    reparentingpmwidget.cpp
    scriptablereparentingwidget.cpp
  )
endif()

find_package(Qt${QT_MAJOR_VERSION}QuickWidgets ${REQUIRED_QT_VERSION} CONFIG)
if (Qt${QT_MAJOR_VERSION}QuickWidgets_FOUND AND NOT Qt${QT_MAJOR_VERSION}QuickWidgets_VERSION VERSION_LESS 5.5)
  list(APPEND proxymodeltestapp_SRCS
    selectioninqmlwidget.cpp
  )
  add_definitions(-DSRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
endif()

add_executable(proxymodeltestapp ${proxymodeltestapp_SRCS})

target_link_libraries(proxymodeltestapp
   KF5::ItemModels
   proxymodeltestsuite
   Qt${QT_MAJOR_VERSION}::Widgets
)

if (Qt${QT_MAJOR_VERSION}Script_FOUND)
  target_link_libraries(proxymodeltestapp
    Qt${QT_MAJOR_VERSION}::Script
  )
endif()
if (Qt${QT_MAJOR_VERSION}QuickWidgets_FOUND AND NOT Qt${QT_MAJOR_VERSION}QuickWidgets_VERSION VERSION_LESS 5.5)
  target_link_libraries(proxymodeltestapp
    Qt${QT_MAJOR_VERSION}::QuickWidgets
  )
endif()
