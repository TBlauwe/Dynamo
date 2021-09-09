//
// Created by Tristan on 09/09/2021.
//

#include "persona.hpp"
#include <iostream>

namespace Engine{
    void initialize(){
        std::cout << "Initialization complete !" << std::endl;
    }

    void run(){
        //spdlog::info("shouting ...");
    }

    void shutdown(){
        std::cout << "Shutdown complete !" << std::endl;
    }
}