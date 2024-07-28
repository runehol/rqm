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

static void RQM_add_same_sign(benchmark::State &state)
{
    // Perform setup here
    rqm::num a = 0x123456789;
    rqm::num b = 0x123456789;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        rqm::num c = a + b;
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(RQM_add_same_sign);

static void RQM_add_diff_sign_a_larger(benchmark::State &state)
{
    // Perform setup here
    rqm::num a = 0x123456789;
    rqm::num b = -0x12345678;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        rqm::num c = a + b;
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(RQM_add_diff_sign_a_larger);

static void RQM_add_diff_sign_b_larger(benchmark::State &state)
{
    // Perform setup here
    rqm::num a = -0x12345678;
    rqm::num b = 0x123456789;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        rqm::num c = a + b;
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(RQM_add_diff_sign_b_larger);

static void RQM_mul(benchmark::State &state)
{
    // Perform setup here
    rqm::num a = 0x123456789;
    rqm::num b = 0x12345678;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        rqm::num c = a * b;
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(RQM_mul);

static void RQM_mul_num_with_digit(benchmark::State &state)
{
    // Perform setup here
    rqm::num a = 0x123456789;
    int32_t b = 0x12345678;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        rqm::num c = a * b;
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(RQM_mul_num_with_digit);

static void RQM_div_num_with_digit(benchmark::State &state)
{
    // Perform setup here
    rqm::num a = 0x123456789;
    int32_t b = 0x12345678;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        rqm::num c = a / b;
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(RQM_div_num_with_digit);
