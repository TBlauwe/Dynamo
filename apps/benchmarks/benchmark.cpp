#include <benchmark/benchmark.h>
#include <dynamo/dynamo.hpp>

static void BM_creating_simulation_empty(benchmark::State& state) {
    for (auto _ : state) {
        flecs::world world{};
        world.import<dynamo::Simulation>();
    }
}

BENCHMARK(BM_creating_simulation_empty);

// Run the benchmark
BENCHMARK_MAIN();