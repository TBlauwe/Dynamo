add_library(persona STATIC persona.cpp)
target_link_libraries(persona PUBLIC doctest spdlog::spdlog flecs_static)
