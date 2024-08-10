add_library(rqm STATIC)
target_include_directories(rqm PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_sources(rqm PRIVATE
	basic_arithmetic.cpp
	string_conversion.cpp
	znum.cpp
)
