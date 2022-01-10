#include <application/application.h>
#include <dynamo/strategies/all.hpp>
#include <dynamo/simulation.hpp>
#include <dynamo/modules/basic_stress.hpp>
#include <dynamo/gui/flecs_inspector.hpp>
#include <dynamo/gui/dynamo_inspector.hpp>

using namespace dynamo;

struct Reglementary {};

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

        //auto v = static_value<std::vector<int>>(0, 1, 2, 3, 4, 5);
        //v.name("Integers Vector");

        //auto p_d = process<strat::InfluenceGraph, int, std::vector<int>>(v);
        //p_d.name("Integer selection");
        //p_d.input_name(v, "Integers");

        //auto p_e = process<strat::Random, std::vector<Action>>();
        //p_e.name("Potential actions");

        //auto p_f = process<strat::InfluenceGraph, Action, std::vector<Action>>(p_e);
        //p_f.name("Action selection");
        //p_f.input_name(p_e, "Actions");
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
                [](AgentHandle agent, const std::string& arg, const int& arg2) {
                    return "Oups " + arg;
                }
            )
            .behaviour(
                "MySecondBehaviour",
                [](AgentHandle agent) {return true; },
                [](AgentHandle agent, const std::string& arg, const int& arg2) {
                    return "Arff " + arg;
                }
            )
                    ;

        sim.strategy<strat::InfluenceGraph<int, std::vector<int>>>()
            .behaviour(
                "WantEven",
                [](AgentHandle agent) {return true; },
                [](AgentHandle agent, const std::vector<int>& arg){
                    std::vector<dynamo::Influence<int>> output;
                    for (const auto& v : arg)
                    {
                        output.emplace_back(&v, !(v % 2));
                    }
                    return output;
                }
            )
            ;

        sim.strategy<strat::Random<std::vector<Action>>>()
            .behaviour(
                "MyFirstBehaviour",
                [](AgentHandle agent) {return true; },
                [](AgentHandle agent) {
                    std::vector<Action> output {};
                    agent.entity().world().each([&output](flecs::entity e, const type::Action& _) {
                        output.emplace_back(e);
                        });
                    return output;
                }
            );

        sim.strategy<strat::InfluenceGraph<Action, std::vector<Action>>>()
            .behaviour(
                "Followership passive",
                [](AgentHandle agent) {return true; },
                [](AgentHandle agent, const std::vector<Action>& arg){
                    std::vector<dynamo::Influence<Action>> output;
                    for (const auto& v : arg)
                    {
                        if(v.has<type::Ordered>())
                            output.emplace_back(&v, true);
                    }
                    return output;
                }
            )
            .behaviour(
                "Stress",
                [](AgentHandle agent) {return is_pressured(agent); },
                [](AgentHandle agent, const std::vector<Action>& arg){
                    std::vector<dynamo::Influence<Action>> output;
                    for (const auto& v : arg)
                    {
                        if(!agent.has<type::Qualification>() && !v.has<type::Qualification>())
                            output.emplace_back(&v, true);
                        else if(!agent.has<type::Qualification>() && v.has<type::Qualification>())
                            output.emplace_back(&v, false);
                        else if(v.has<type::Qualification>() && (agent.get<type::Qualification>()->value >= v.get<type::Qualification>()->value));
                            output.emplace_back(&v, true);
                    }
                    return output;
                }
            )
            .behaviour(
                "Reglementary",
                [](AgentHandle agent) {return is_pressured(agent) && agent.has<Reglementary>(); },
                [](AgentHandle agent, const std::vector<Action>& arg){
                    std::vector<dynamo::Influence<Action>> output;
                    for (const auto& v : arg)
                    {
                        if(v.has<type::ReglementaryCost>())
                            output.emplace_back(&v, true);
                    }
                    return output;
                }
            )
            ;

        // -- Register some processes/reasonner
        // You must register them before populating the simulation.
        sim.agent_model<SimpleReasonner>();


        // -- Create some entities to populate the simulation;
        // First, let's create a prefab for our agents, or an archetype :
        auto archetype = sim.agent_archetype("Archetype_Basic")
            .add<type::Stress>()
            .add<type::Qualification>()
            .agent_model<SimpleReasonner>()
            ;

        sim.action("Rester en alerte")
            .set<type::Cost>({0})
            ;
        sim.action("Poser un garrot avec un bout de tissu")
            .set<type::Cost>({1})
            .set<type::Qualification>({3})
            ;
        sim.action("Poser un pansement compressif")
            .set<type::Cost>({3})
            .set<type::Qualification>({2})
            ;
        sim.action("Poser un garrot rapidement")
            .set<type::Cost>({ 2 })
            .add<type::Ordered>()
            .set<type::Qualification>({1})
            ;
        sim.action("Poser un garrot dans les règles de l'art")
            .set<type::Cost>({2})
            .set<type::ReglementaryCost>({1})
            .set<type::Qualification>({2})
            ;

        // Then, we can create agent using our archetype :
        for (int i = 0; i < number_of_agents; i++) {
            auto agent = sim.agent(archetype, fmt::format("Agent {}", i).c_str())
                .set<type::Qualification>({rand()%3+1});
            if (rand() % 2)
                agent.add<Reglementary>();
        }

        std::vector<flecs::entity_view> agents {};
        sim.for_each([&agents](flecs::entity agent, const dynamo::type::Agent _){
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