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
