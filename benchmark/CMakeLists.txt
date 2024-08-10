add_executable(benchmark_rqm)

enable_sanitizers(benchmark_rqm)

target_sources(benchmark_rqm PRIVATE
		benchmark_main.cpp
		benchmark_znum.cpp
	)

target_link_libraries(benchmark_rqm PRIVATE benchmark::benchmark)
target_link_libraries(benchmark_rqm PRIVATE rqm)
target_link_libraries(benchmark_rqm PRIVATE project_options project_warnings)


# if we can find gmp on the system, include the gmp benchmarks
find_library(GMP_LIB gmp)
find_path(GMP_INCLUDE_DIR
    NAMES gmp.h
)

if(GMP_LIB AND GMP_INCLUDE_DIR)
	message(STATUS "Found gmp library: ${GMP_LIB}")
	target_link_libraries(benchmark_rqm PRIVATE ${GMP_LIB})
	target_include_directories(benchmark_rqm PRIVATE ${GMP_INCLUDE_DIR})
	target_sources(benchmark_rqm PRIVATE benchmark_gmp.cpp)
else()
	message(STATUS "gmp not found")
endif()





add_custom_target(run_benchmark
COMMAND benchmark_rqm
DEPENDS benchmark_rqm
USES_TERMINAL
)
