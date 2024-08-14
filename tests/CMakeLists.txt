include( CTest )

include(GoogleTest)

add_executable(test_rqm)

enable_sanitizers(test_rqm)

target_sources(test_rqm PRIVATE
		test_znum.cpp
		test_qnum.cpp
	)

target_link_libraries(test_rqm PRIVATE gtest_main)
target_link_libraries(test_rqm PRIVATE rapidcheck_gtest gtest)
target_link_libraries(test_rqm PRIVATE rqm)
target_link_libraries(test_rqm PRIVATE project_options project_warnings)

gtest_discover_tests(test_rqm WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

set(CMAKE_CTEST_COMMAND ctest --progress --output-on-failure)

add_custom_target(check
COMMAND ${CMAKE_CTEST_COMMAND}
DEPENDS test_rqm
USES_TERMINAL
)
