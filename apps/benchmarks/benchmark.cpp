#include <benchmark/benchmark.h>
#include <dynamo/simulation.hpp>

const size_t repetitions_count = 10;

static void BM_create_simulation_empty(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        auto sim = dynamo::Simulation();
    }
}
BENCHMARK(BM_create_simulation_empty)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(repetitions_count)->DisplayAggregatesOnly()
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

static void BM_create_agent(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        state.PauseTiming();
        auto sim = dynamo::Simulation();
        state.ResumeTiming();
        sim.agent();
    }
}
BENCHMARK(BM_create_agent)
        ->Unit(benchmark::kMillisecond)
        ->Repetitions(repetitions_count)->DisplayAggregatesOnly()
;

static void BM_create_n_agent(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        state.PauseTiming();
        auto sim = dynamo::Simulation();
        state.ResumeTiming();
        for(int i = 0; i<state.range(0); i++){
            sim.agent();
        }
    }
}
BENCHMARK(BM_create_n_agent)
        ->Unit(benchmark::kMillisecond)
        ->Range(1<<0, 1<<14)
;

static void BM_create_percept(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        state.PauseTiming();
        auto sim = dynamo::Simulation();
        auto artefact = sim.artefact();
        state.ResumeTiming();
        sim.percept(artefact);
    }
}
BENCHMARK(BM_create_percept)
        ->Unit(benchmark::kMillisecond)
        ->Repetitions(repetitions_count)->DisplayAggregatesOnly()
;

// Run the benchmark
BENCHMARK_MAIN();