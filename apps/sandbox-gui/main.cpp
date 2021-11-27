#include <application/application.h>
#include <dynamo/strategies/all.hpp>
#include <dynamo/simulation.hpp>
#include <dynamo/modules/basic_stress.hpp>
#include <dynamo/gui/flecs_inspector.hpp>
#include <dynamo/gui/dynamo_inspector.hpp>

using namespace dynamo;

class SimpleReasonner : public AgentModel
{
public:
    using AgentModel::AgentModel;

    virtual constexpr const char* name() const { return "MySuperReasonner"; }

    void build() override
    {
        // -- Create simple tasks with no behaviours
        // Empty task
        auto t0 = emplace([](AgentHandle agent) {});
        t0.name("Empty task");

        // Task mutating the agent (deferred)
        auto t1 = emplace([](AgentHandle agent) { agent.set<type::Stress>({ 9.0f }); });
        t1.name("Set stress to 9");

        // Task also mutating the agent (deferred)
        auto t2 = emplace([](AgentHandle agent) { agent.set<type::Stress>({ 12.0f }); });
        t2.name("Set stress to 12");

        // For t0 to launch, t1 and t2 must be finished
        t0.succeed(t1, t2);

        // -- Create complex tasks with changing behaviour
        // Process with a random strategy that returns a string
        // and has no inputs
        // NOTE: A random strategy selects randomly a behaviour
        // and execute it. Complex strategy can be defined to take
        // into account one or multiple behaviours
        auto p_a = process<strat::Random, std::string>();
        p_a.name("Random string");
        p_a.succeed(t0); // This process is dependant of t0

        // Process with a random strategy that returns an int
        // and has no inputs
        auto p_b = process<strat::Random, int>();
        p_b.name("Random int");

        // Process with a random strategy that returns a string
        // and has two inputs (string and int)
        auto p_c = process<strat::Random, std::string, std::string, int>(p_a, p_b);
        p_c.name("Aggregate");
        p_c.input_name(p_a, "A string");
        p_c.input_name(p_b, "An integer");

        auto v = static_value<std::vector<int>>(0, 1, 2, 3, 4, 5);
        v.name("Vector");

        //auto p_d = process<strat::InfluenceGraph, int, std::vector<int>>(v);
        //p_d.name("Action selection");
        //p_d.input_name(v, "Actions");
    }
};


class Sandbox : public app::Application{

private:
    dynamo::Simulation      sim{};
    FlecsInspector          flecs_inspector     {sim.world()};
    dynamo::DynamoInspector dynamo_inspector    {sim};

    bool    is_enabled      {false};
    bool    step_once       {false};
    float   timescale       {1.0f};
    float   min_timescale   {.05f};
    float   max_timescale   {10.0f};

public:
    Sandbox() :
        Application(1280,720,"Sandbox", "Sandbox")
    {
        const size_t number_of_agents = 10;
        // -- Setup
        sim.world().import<module::BasicStress>();

        // -- Define strategies and behaviours
        sim.strategy<strat::Random<std::string>>()
            .behaviour(
                "MyFirstBehaviour",
                [](AgentHandle agent) {return true; },
                [](AgentHandle agent) {return "Yeah"; }
            )
            .behaviour(
                "MySecondBehaviour",
                [](AgentHandle agent) {return true; },
                [](AgentHandle agent) {return "Nay (but Yeah!)"; }
        );

        sim.strategy<strat::Random<int>>()
            .behaviour(
                "MyFirstBehaviour",
                [](AgentHandle agent) {return true; },
                [](AgentHandle agent) {return 0; }
            )
            .behaviour(
                "MySecondBehaviour",
                [](AgentHandle agent) {return true; },
                [](AgentHandle agent) {return 1; }
        );

        sim.strategy<strat::Random<std::string, std::string, int>>()
            .behaviour(
                "MyFirstBehaviour",
                [](AgentHandle agent) {return true; },
                [](AgentHandle agent, std::string arg, int arg2) {
                    return "Oups " + arg;
                }
            )
            .behaviour(
                "MySecondBehaviour",
                [](AgentHandle agent) {return true; },
                [](AgentHandle agent, std::string arg, int arg2) {
                    return "Arff " + arg; }
                );

        //auto& influence_graph_strat = sim.add<strat::InfluenceGraph<int>>();
        //influence_graph_strat.add(Behaviour<std::vector<dynamo::strat::Influence<int>>, const std::vector<int const*>&>{
        //    "MyOtherBehaviour",
        //        [](AgentHandle agent) -> bool {return true; },
        //        [](AgentHandle agent) -> std::string {return "Yeah"; }
        //});

        // -- Register some processes/reasonner
        // You must register them before populating the simulation.
        sim.agent_model<SimpleReasonner>();


        // -- Create some entities to populate the simulation;
        // First, let's create a prefab for our agents, or an archetype :
        auto archetype = sim.agent_archetype("Archetype_Basic")
            .add<type::Stress>()
            .agent_model<SimpleReasonner>()
            ;

        // Then, we can create agent using our archetype :
        auto arthur = sim.agent(archetype, "Arthur");
        for (int i = 0; i < number_of_agents; i++) {
            sim.agent(archetype, fmt::format("Agent {}", i).c_str());
        }

        std::vector<flecs::entity_view> agents {};
        sim.for_each([&agents](flecs::entity agent, dynamo::type::Agent& _){
            agents.emplace_back(agent);
        });

        sim.artefact("Radio").entity()
            .set<dynamo::type::PeriodicEmitter, dynamo::type::Message>({0.5f})
            .set<dynamo::type::Targets>({agents})
            ;
    }

private:
    void on_menu_bar() override{
        ImGui::Separator();
        if(is_enabled){
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.3f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.3f, 0.8f, 0.8f));
        }else{
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.1f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.1f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.1f, 0.8f, 0.8f));
            if(ImGui::Button("  " ICON_FA_STEP_FORWARD " Step  ")){
                step_once = true;
            }
            ImGui::PopStyleColor(3);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));
        }
        if(ImGui::Button(!is_enabled ? "  " ICON_FA_PAUSE " Paused  " : "  " ICON_FA_PLAY " Playing  ")){
            is_enabled = !is_enabled;
        }
        ImGui::PopStyleColor(3);

        ImGui::Separator();
        ImGui::SetNextItemWidth(150.f);
        if(ImGui::SliderFloat("TimeScale", &timescale, min_timescale, max_timescale)){
            sim.world().set_time_scale(timescale);
        }
        ImGui::Widgets::HelpMarker("Ctrl + Click to input a specific value");
        ImGui::Separator();
    }

    void on_update() override{
        flecs_inspector.show();
        dynamo_inspector.show();
        if(is_enabled || step_once){
            sim.step(ImGui::GetIO().DeltaTime);
            step_once = false;
        }
    }
};

int main(){
    Sandbox sandbox;
    sandbox.run();
}