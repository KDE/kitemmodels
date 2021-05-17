project(bihash_test)

include_directories("../../src/core") # for kbihash_p.h

set(functionalitytest_srcs
  functionalitytest.cpp
)

add_executable(bihash_functionalitytest ${functionalitytest_srcs})

target_link_libraries(bihash_functionalitytest
  Qt5::Core
)

set(benchmarks_srcs
  benchmarks.cpp
)

add_executable(bihash_benchmarks_exec ${benchmarks_srcs} )

target_link_libraries(bihash_benchmarks_exec
  Qt5::Widgets
  Qt5::Test
)
