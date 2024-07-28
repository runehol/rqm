#include <benchmark/benchmark.h>
#include <stdio.h>

#include <gmp.h>

std::string to_string(mpz_t v)
{
    char buf[1024];
    gmp_snprintf(buf, 1024, "%Zd", v);
    return std::string(buf);
}

static void GMP_add_same_sign(benchmark::State &state)
{
    mpz_t a, b, c;
    mpz_inits(a, b, c, nullptr);

    // Perform setup here
    mpz_set_str(a, "0x123456789", 0);
    mpz_set_str(b, "0x123456789", 0);

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        mpz_add(c, a, b);
        benchmark::DoNotOptimize(c);
    }
    assert(to_string(c) == "9773436690");
    mpz_clears(a, b, c, nullptr);
}

BENCHMARK(GMP_add_same_sign);

static void GMP_add_diff_sign_a_larger(benchmark::State &state)
{
    mpz_t a, b, c;
    mpz_inits(a, b, c, nullptr);

    // Perform setup here
    mpz_set_str(a, "0x123456789", 0);
    mpz_set_str(b, "-0x12345678", 0);

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        mpz_add(c, a, b);
        benchmark::DoNotOptimize(c);
    }
    assert(to_string(c) == "4581298449");
    mpz_clears(a, b, c, nullptr);
}

BENCHMARK(GMP_add_diff_sign_a_larger);

static void GMP_add_diff_sign_b_larger(benchmark::State &state)
{
    mpz_t a, b, c;
    mpz_inits(a, b, c, nullptr);

    // Perform setup here
    mpz_set_str(a, "-0x12345678", 0);
    mpz_set_str(b, "0x123456789", 0);

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        mpz_add(c, a, b);
        benchmark::DoNotOptimize(c);
    }
    assert(to_string(c) == "4581298449");
    mpz_clears(a, b, c, nullptr);
}

BENCHMARK(GMP_add_diff_sign_b_larger);

static void GMP_mul(benchmark::State &state)
{
    mpz_t a, b, c;
    mpz_inits(a, b, c, nullptr);

    // Perform setup here
    mpz_set_str(a, "0x123456789", 0);
    mpz_set_str(b, "0x12345678", 0);

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        mpz_mul(c, a, b);
        benchmark::DoNotOptimize(c);
    }
    assert(to_string(c) == "1492501008711192120");
    mpz_clears(a, b, c, nullptr);
}

BENCHMARK(GMP_mul);

static void GMP_mul_num_with_digit(benchmark::State &state)
{
    mpz_t a, c;
    mpz_inits(a, c, nullptr);
    // Perform setup here
    mpz_set_str(a, "0x123456789", 0);
    int32_t b = 0x12345678;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        mpz_mul_si(c, a, b);
        benchmark::DoNotOptimize(c);
    }
    assert(to_string(c) == "1492501008711192120");
    mpz_clears(a, c, nullptr);
}

BENCHMARK(GMP_mul_num_with_digit);

static void GMP_div_num_with_digit(benchmark::State &state)
{
    mpz_t a, c;
    mpz_inits(a, c, nullptr);
    // Perform setup here
    mpz_set_str(a, "0x123456789", 0);
    int32_t b = 0x12345678;

    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    for(auto _: state)
    {
        // This code gets timed
        mpz_tdiv_q_ui(c, a, b);
        benchmark::DoNotOptimize(c);
    }
    assert(to_string(c) == "16");
    mpz_clears(a, c, nullptr);
}

BENCHMARK(GMP_div_num_with_digit);
