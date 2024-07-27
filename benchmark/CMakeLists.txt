add_executable(benchmark_rqm)

enable_sanitizers(benchmark_rqm)

target_sources(benchmark_rqm PRIVATE
		benchmark_main.cpp
		benchmark_rqm.cpp
	)

target_link_libraries(benchmark_rqm PRIVATE benchmark::benchmark)
target_link_libraries(benchmark_rqm PRIVATE rqm)
target_link_libraries(benchmark_rqm PRIVATE project_options project_warnings)

add_custom_target(run_benchmark
COMMAND benchmark_rqm
DEPENDS benchmark_rqm
USES_TERMINAL
)
