add_library(proxymodeltestsuite SHARED)
add_library(KF5::proxymodeltestsuite ALIAS proxymodeltestsuite)

target_sources(proxymodeltestsuite PRIVATE
    dynamictreemodel.cpp
    dynamictreewidget.cpp
    modelcommander.cpp
    modelspy.cpp
    modelselector.cpp
    modeltest.cpp
    proxymodeltest.cpp
    modeldumper.cpp
    modeleventlogger.cpp
    eventloggerregister.cpp

    eventlogger.qrc
)

if(FALSE)
  # Grantlee is used for generating compilable code by the ModelEventLogger.
  # If Grantlee is not found, the logger does nothing.
  #find_package(Grantlee QUIET NO_MODULE)
  # TODO: Put this part in Grantlee
  set_package_properties(Grantlee PROPERTIES
    URL "http://www.grantlee.org"
    DESCRIPTION "A String Template system for Qt"
  )

  set_package_properties(Grantlee PROPERTIES
    TYPE OPTIONAL
    PURPOSE "Grantlee is used for generating compilable code by the ModelEventLogger. Without Grantlee, the logger will do nothing."
  )
endif()

if (Grantlee_FOUND)

  target_include_directories(proxymodeltestsuite ${Grantlee_INCLUDE_DIRS})

  configure_file(grantlee_paths.h.cmake ${PROJECT_BINARY_DIR}/grantlee_paths.h)

  target_compile_definitions(proxymodeltestsuite "-DGrantlee_FOUND")

endif()

generate_export_header(proxymodeltestsuite)

target_link_libraries(proxymodeltestsuite
   Qt5::Widgets
   Qt5::Test
   ${Grantlee_CORE_LIBRARIES}
)

