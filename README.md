![example workflow](https://github.com/TBlauwe/Dynamo/actions/workflows/Build.yml/badge.svg)

# Dynamo

## Build

### Prerequisites
1. Download and install Vulkan 
   * https://vulkan.lunarg.com/sdk/home
   * __Don't forget to install 32bit libraries if you want to build for 32 targets !__

### Cmake
3. Download source code
4. Build targets using your favorite IDE
   * __OPTIONS__:
     * BUILD_DYNAMO_GUI: 
       * Library to debug dynamo using imgui
       * Target: dynamo-gui
     * BUILD_APPS
       * Build several applications using dynamo
     * 