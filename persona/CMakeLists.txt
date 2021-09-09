MESSAGE(STATUS "Building Persona")

add_library(persona STATIC persona.cpp)
target_link_libraries(persona PRIVATE doctest)
