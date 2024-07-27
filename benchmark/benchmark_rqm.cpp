#include <benchmark/benchmark.h>


static void BM_add(benchmark::State& state) {
    // Perform setup here
    int a = 2;
    int b = 1;

    size_t n_iters = 0;
    for (auto _ : state) {
        // This code gets timed
        int c = a + b;
        benchmark::DoNotOptimize(c);
        ++n_iters;

    }
    state.SetItemsProcessed(n_iters);
}

BENCHMARK(BM_add);
