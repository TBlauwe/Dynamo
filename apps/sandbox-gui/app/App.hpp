#ifndef DYNAMO_APP_HPP
#define DYNAMO_APP_HPP

#include <application/application.h>
#include <spdlog/spdlog.h>
#include <dynamo/dynamo.hpp>
#include <dynamo/gui/debug.hpp>


class Sandbox : public app::Application{
public:
    Sandbox():
            Application(1280,720,"Sandbox", "Sandbox"),
            sim(),
            number_perception(sim.world)
    {
        auto arthur = sim.world.entity("Arthur");
        auto bruno = sim.world.entity("Bruno");
        logger->info("Arthur id : {}", arthur);
        logger->info("Bruno id : {}", bruno);
        number_perception.add_perception(arthur);
        number_perception.add_perception(bruno);
    }

private:
    dynamo::Simulation sim;
    dynamo::Perception<int> number_perception;

private:
    void on_update() override {
        sim.run();
        ImGui::Begin("Dynamo");

        static int number = 0;
        static int entity = 0;
        ImGui::DragInt("Number", &number);
        ImGui::DragInt("Entity", &entity);
        if(ImGui::Button("Send")){
            number_perception.add_percept(flecs::entity(sim.world, entity), number);
        }

        sim.world.each([](flecs::entity e, const dynamo::Perception<int>::Buffer& buffer) {
            ImGui::Text("[%llu] %s", e.id(), e.name().c_str());
            ImGui::Text("Perceptions :");
            ImGui::Indent();
            for(auto number : buffer.data){
                ImGui::BulletText("%d", number);
            }
            ImGui::Unindent();
        });
        ImGui::End();
    }
};

#endif //DYNAMO_APP_HPP