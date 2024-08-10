#include "rqm/rqm.h"
#include <benchmark/benchmark.h>

static void int64_t_add(benchmark::State &state)
{
    // Perform setup here
    int64_t a = 99999999;
    int64_t b = -999999999;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        int64_t c = a + b;
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(int64_t_add);

static void int64_t_mul(benchmark::State &state)
{
    // Perform setup here
    int64_t a = 99999999;
    int64_t b = -999999999;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        int64_t c = a * b;
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(int64_t_mul);

static void int64_t_div(benchmark::State &state)
{
    // Perform setup here
    int64_t a = 99999999;
    int64_t b = -999999999;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        int64_t c = a / b;
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(int64_t_div);

static void RQM_ZNUM_add_same_sign(benchmark::State &state)
{
    // Perform setup here
    rqm::znum a = 0x123456789;
    rqm::znum b = 0x123456789;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    rqm::znum c;
    for(auto _: state)
    {
        // This code gets timed
        c = a + b;
        benchmark::DoNotOptimize(c);
    }
    assert(rqm::to_string(c) == "9773436690");
}

BENCHMARK(RQM_ZNUM_add_same_sign);

static void RQM_ZNUM_add_diff_sign_a_larger(benchmark::State &state)
{
    // Perform setup here
    rqm::znum a = 0x123456789;
    rqm::znum b = -0x12345678;
    rqm::znum c;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        c = a + b;
        benchmark::DoNotOptimize(c);
    }
    assert(rqm::to_string(c) == "4581298449");
}

BENCHMARK(RQM_ZNUM_add_diff_sign_a_larger);

static void RQM_ZNUM_add_diff_sign_b_larger(benchmark::State &state)
{
    // Perform setup here
    rqm::znum a = -0x12345678;
    rqm::znum b = 0x123456789;
    rqm::znum c;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        c = a + b;
        benchmark::DoNotOptimize(c);
    }
    assert(rqm::to_string(c) == "4581298449");
}

BENCHMARK(RQM_ZNUM_add_diff_sign_b_larger);

static void RQM_ZNUM_mul(benchmark::State &state)
{
    // Perform setup here
    rqm::znum a = 0x123456789;
    rqm::znum b = 0x12345678;
    rqm::znum c;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        c = a * b;
        benchmark::DoNotOptimize(c);
    }
    assert(rqm::to_string(c) == "1492501008711192120");
}

BENCHMARK(RQM_ZNUM_mul);

static void RQM_ZNUM_mul_num_with_digit(benchmark::State &state)
{
    // Perform setup here
    rqm::znum a = 0x123456789;
    rqm::znum c;
    int32_t b = 0x12345678;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        c = a * b;
        benchmark::DoNotOptimize(c);
    }
    assert(rqm::to_string(c) == "1492501008711192120");
}

BENCHMARK(RQM_ZNUM_mul_num_with_digit);

static void RQM_ZNUM_div(benchmark::State &state)
{
    // Perform setup here
    rqm::znum a = 0x123456789;
    rqm::znum c;
    rqm::znum b = 0x12345678;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        c = a / b;
        benchmark::DoNotOptimize(c);
    }
    assert(rqm::to_string(c) == "16");
}

BENCHMARK(RQM_ZNUM_div);

static void RQM_ZNUM_div_num_with_digit(benchmark::State &state)
{
    // Perform setup here
    rqm::znum a = 0x123456789;
    rqm::znum c;
    int32_t b = 0x12345678;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        c = a / b;
        benchmark::DoNotOptimize(c);
    }
    assert(rqm::to_string(c) == "16");
}

BENCHMARK(RQM_ZNUM_div_num_with_digit);
