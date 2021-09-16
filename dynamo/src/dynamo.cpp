//
// Created by Tristan on 09/09/2021.
//
#include <dynamo/dynamo.hpp>

dynamo::Simulation::Simulation() :
        world()
{
}

void dynamo::Simulation::run(){
    world.progress();
}

void dynamo::Simulation::shutdown(){
}