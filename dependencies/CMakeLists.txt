# fetch 3rd party libraries
include(FetchContent)

set(CMAKE_CXX_CLANG_TIDY "") # turn off clang-tidy for third-party code

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE INTERNAL "")
set(INSTALL_GTEST OFF CACHE INTERNAL "")


FetchContent_Declare(
  googletest
  URL               https://github.com/google/googletest/archive/refs/tags/v1.15.0.tar.gz
  URL_MD5           1fb60c818501bcecee87349cf2e7377f

)
FetchContent_MakeAvailable(googletest)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE INTERNAL "")
set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")

FetchContent_Declare(
  googlebenchmark
  URL               https://github.com/google/benchmark/archive/refs/tags/v1.8.5.tar.gz
  URL_MD5           708d91ce255e8af4c1d7dfec50dff178

)
FetchContent_MakeAvailable(googlebenchmark)

set(RC_ENABLE_GTEST ON CACHE INTERNAL "")

FetchContent_Declare(
  rapidcheck
  GIT_REPOSITORY    https://github.com/emil-e/rapidcheck
  GIT_TAG           ff6af6fc683159deb51c543b065eba14dfcf329b

)
FetchContent_MakeAvailable(rapidcheck)
