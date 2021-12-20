project(bihash_test)

include_directories("../../src/core") # for kbihash_p.h

add_executable(bihash_functionalitytest functionalitytest.cpp)

target_link_libraries(bihash_functionalitytest
  Qt${QT_MAJOR_VERSION}::Core
)

add_executable(bihash_benchmarks_exec benchmarks.cpp)

target_link_libraries(bihash_benchmarks_exec
  Qt${QT_MAJOR_VERSION}::Widgets
  Qt${QT_MAJOR_VERSION}::Test
)
