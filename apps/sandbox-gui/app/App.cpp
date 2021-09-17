#include "App.hpp"
#include <implot.h>
#include <vector>
#include <array>
#include <numeric>
#include <boost/circular_buffer.hpp>

namespace app
{
    void on_initialization(){
    }

    void on_shutdown(){
    }
    namespace ImGuiWindows{
        void menu_bar(){
        }
    }
}

Sandbox::Sandbox() :
    Application(1280,720,"Sandbox", "Sandbox"),
    sim(),
    number_perception(sim.world)
{
    auto arthur = sim.add_agent("Arthur");
    logger->info("Arthur id : {}", arthur);
    auto  bruno = sim.add_agent("Bruno");
    logger->info("Bruno id : {}", bruno);
}

void Sandbox::on_update() {
    sim.run();
    ImGui::Begin("Dynamo");

    static int number = 0;
    static int entity = 0;
    static boost::circular_buffer<int> cb(1000);
    ImGui::DragInt("Number", &number);
    ImGui::DragInt("Entity", &entity);
    if(ImGui::Button("Send")){
        //number_perception.add_percept<dynamo::DecayingPercept, int>({2.0f}, 1);
        auto e = sim.world.entity();
        e.set<dynamo::DecayingPercept>({2.0f});
        e.set<int>(2);
    }

    sim.world.each([](flecs::entity e, const dynamo::Agent& agent) {
        ImGui::Text("[%llu] %s", e.id(), e.name().c_str());
    });

    cb.push_back(sim.world.count<const dynamo::DecayingPercept>());
    cb.linearize();
    std::vector<int> tick(cb.size());
    std::iota(tick.begin(), tick.end(), 0);
    ImPlot::SetNextPlotLimits(0,cb.size(),0,20);
    ImPlot::FitNextPlotAxes(true, true);
    if (ImPlot::BeginPlot("Perceptions plot", "tick", "count")) {
        ImPlot::PlotLine("percepts", tick.data(), cb.array_one().first, cb.size());
        ImPlot::Annotate(100, 5, ImVec2(10,10),ImPlot::GetLastItemColor(),"Stressed");
        ImPlot::EndPlot();
    }
    ImGui::End();
}

