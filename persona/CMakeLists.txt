MESSAGE(STATUS " >>>>> Building Persona <<<<<")

add_library(persona STATIC persona.cpp)
target_link_libraries(persona PUBLIC doctest spdlog::spdlog_header_only flecs_static)
