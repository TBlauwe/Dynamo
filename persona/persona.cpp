//
// Created by Tristan on 09/09/2021.
//

#include "persona.hpp"
#include <iostream>
#include <doctest/doctest.h>
namespace Engine{
    void initialize(){
        std::cout << "Initialization complete !" << std::endl;
    }

    void run(){
        std::cout << "shouting ..." << std::endl;
    }

    void shutdown(){
        std::cout << "Shutdown complete !" << std::endl;
    }
}

TEST_CASE("testing inside static lib") {
    CHECK(true);
}