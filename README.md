![wip](https://img.shields.io/badge/-WIP-red)
![example workflow](https://github.com/TBlauwe/Dynamo/actions/workflows/Build.yml/badge.svg)

# Dynamo

C++ library to simulate agents evolving withing a world. Agents run according to an agent model that you specified.

> :warning: Project not ready yet !

```cpp
#include <dynamo/simulation.hpp>

// ... somewhere in your code
auto sim = dynamo::Simulation(); // Initialize an empty simulation

// ... define domain data
// ... define cognitive models
// ... define agent model

auto artefact   = sim.artefact("An artefact");  // Create an artefact.
auto arthur     = sim.agent("Arthur");          // Create an agent.
sim.percept<your::senses>(artefact)             // Create a percept coming from artefact, perceived by arthur.
    .perceived_by(arthur); 

sim.step(); // advance simulation by one step
sim.step(1.0f); // advance simulation by one step as if 1 second has passed
sim.step_n(100); // advance simulation by 100 steps
sim.step_n(100, 1.0f); // advance simulation by 100 steps, as if 1 seconds passed between each steps
```

## Installation

> :warning: needs to be tested !
 
To use the library, include the library in your build system :

### CMake

We recommend to use [CPM](https://github.com/cpm-cmake/CPM.cmake)
```cmake
CPMAddPackage(
        NAME dynamo 
        GITHUB_REPOSITORY TBlauwe/Dynamo
        OPTIONS
            "DYN_BUILD_DYNAMO_GUI OFF"
            "DYN_BUILD_APPS OFF"
            "DYN_BUILD_BENCHMARKS OFF"
            "DYN_BUILD_DOCS OFF"
            "DYN_BUILD_TESTS OFF"
)
```

#### CMake options

| Options          | Default | Description                                              |
| ---------------- | ------- | -------------------------------------------------------- |
| DYN_BUILD_DYNAMO_GUI | OFF      | Build an optional library to debug visually a simulation |
| DYN_BUILD_APPS      | OFF      | Build several applications to showcasing the library     |
| DYN_BUILD_BENCHMARKS | OFF      | Build benchmarks                                         |
| DYN_BUILD_DOCS       | OFF      | Build documentation                                      |
| DYN_BUILD_TESTS      | OFF      | Build tests                                              |

### Prerequisites

Each target will automatically install dependencies via [CPM](https://github.com/cpm-cmake/).

However, some dependencies needs to be downloaded manually (for some targets) :

#### Apps

> Only if `BUILD_APPS ON` !

Download and install [Vulkan](https://vulkan.lunarg.com/sdk/home) and add it to PATH
Download and install [NASM](https://www.nasm.us/) (only for example apps using grpc) and add it to PATH

:warning: __Don't forget to install 32bit libraries if you want to build for 32bit targets !__

#### Docs

> Only if `BUILD_DOCS ON` !

Install [doxygen](https://www.doxygen.nl/download.html)

If you are seeing this error :

```
error : Problems running epstopdf. Check your TeX installation!
```

Install a TeX distribution on your systems, see : https://www.latex-project.org/get/ .

