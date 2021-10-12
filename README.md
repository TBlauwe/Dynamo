![wip](https://img.shields.io/badge/-WIP-red)
![example workflow](https://github.com/TBlauwe/Dynamo/actions/workflows/Build.yml/badge.svg)

# Dynamo

C++ library to simulate agents evolving withing a world. Agents run according to an agent model that you specified.

```cpp
#include <dynamo/simulation.hpp>

// ... somewhere in your code
auto sim = dynamo::Simulation(); // Initialize an empty simulation

// ... define cognitive models
// ... define domain data
// ... define agent model

sim.step(); // advance simulation by one step
sim.step(1.0f); // advance simulation by one step after (1s have passed inside the simulation)
sim.step_n(100); // advance simulation by 100 steps
sim.step_n(100, 1.0f); // advance simulation by 100 steps of 1s between each steps
```

[TOC]

## Installation

> :warning: needs to be tested !
 
To use the library, include the library in your build system :

### CMake

We recommend to use [CPM](https://github.com/cpm-cmake/CPM.cmake)
```cmake
CPMAddPackage(
        NAME dynamo 
        GITHUB_REPOSITORY TBlauwe/Dynamo/
        OPTIONS
            "BUILD_DYNAMO_GUI OFF"
            "BUILD_APPS OFF"
            "BUILD_BENCHMARKS OFF"
            "BUILD_DOCS OFF"
            "BUILD_TESTS OFF"
)
```

#### CMake options

| Options          | Default | Description                                             |
| ---------------- | ------- | ------------------------------------------------------- |
| BUILD_DYNAMO_GUI | ON      | Build an optional library to debug visualy a simulation |
| BUILD_ APPS      | ON      | Build several applications to showcasing the library    |
| BUILD_BENCHMARKS | ON      | Build benchmarks                                        |
| BUILD_DOCS       | ON      | Build documentation                                     |
| BUILD_TESTS      | ON      | Build tests                                             |

### Prerequisites

Each target will automatically install dependencies via [CPM](https://github.com/cpm-cmake/).

However, some dependencies needs to be downloaded manually (for some targets) :

#### Apps

> Only if `BUILD_APPS ON` !

Download and install [Vulkan](https://vulkan.lunarg.com/sdk/home)

:warning: __Don't forget to install 32bit libraries if you want to build for 32bit targets !__

#### Docs

> Only if `BUILD_DOCS ON` !

Install [doxygen](https://www.doxygen.nl/download.html)

