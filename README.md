![example workflow](https://github.com/TBlauwe/Dynamo/actions/workflows/Build.yml/badge.svg)

# Dynamo

Library to simulate agents evolving withing a world. Agents run according to an agent model that you specified.

```cpp
#include <dynamo/dynamo.hpp>

// ... somewhere in your code
auto sim = dynamo::Simulation(); // Initialize an empty simulation

// ... define cognitive models
// ... define domain data
// ... define agent model

sim.step(); // advance simulation by one step
sim.step_n(100); // advance simulation by 100 steps
```

## Build

### Prerequisites
1. Download and install [Vulkan](https://vulkan.lunarg.com/sdk/home)
   * __Don't forget to install 32bit libraries if you want to build for 32bit targets !__
2. __[OPTIONAL]__ Only if generating doc:
   * Install [doxygen](https://www.doxygen.nl/download.html)

### Cmake
1. Download source code
2. Build targets using your favorite IDE
   * __OPTIONS__:
     * BUILD_DYNAMO_GUI: 
         * default: ON
       * Library to debug dynamo using imgui
       * Target: dynamo-gui
     * BUILD_APPS
         * default: ON
     * BUILD_BENCHMARKS
         * default: ON
     * BUILD_DOCS
         * default: ON
