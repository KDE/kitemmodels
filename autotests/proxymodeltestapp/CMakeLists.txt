
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
    recursivefilterpmwidget.cpp
    descendantpmwidget.cpp
    kreparentingproxymodel.cpp
    proxymodeltestwidget.cpp
    proxyitemselectionwidget.cpp
    lessthanwidget.cpp
    modelcommanderwidget.cpp
    matchcheckingwidget.cpp
)

find_package(Qt5Script ${REQUIRED_QT_VERSION} QUIET CONFIG)
if (Qt5Script_FOUND)
  list(APPEND proxymodeltestapp_SRCS
    reparentingpmwidget.cpp
    scriptablereparentingwidget.cpp
  )
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
