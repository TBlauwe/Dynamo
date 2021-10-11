#include <benchmark/benchmark.h>
#include <dynamo/dynamo.hpp>

static void BM_create_simulation_empty(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        auto sim = dynamo::Simulation();
    }
}
BENCHMARK(BM_create_simulation_empty)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(10)->DisplayAggregatesOnly()
;

static void BM_run_simulation(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        state.PauseTiming();
        auto sim = dynamo::Simulation();
        state.ResumeTiming();
        sim.step_n(state.range(0), 0.5f);
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_run_simulation)
    ->Unit(benchmark::kMillisecond)
    ->RangeMultiplier(2)->Range(1<<0, 1<<8)->Complexity()
    ;

// Run the benchmark
BENCHMARK_MAIN();