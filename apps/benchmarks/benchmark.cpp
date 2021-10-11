#include <benchmark/benchmark.h>
#include <dynamo/dynamo.hpp>

static void BM_endl(benchmark::State& state) {
    for (auto _ : state) {
        flecs::world world{};
        //world.import<dynamo::Simulation>();
    }
}
BENCHMARK(BM_endl);

// Run the benchmark
BENCHMARK_MAIN();