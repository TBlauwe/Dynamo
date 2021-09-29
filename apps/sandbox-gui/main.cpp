#include <application/application.h>
#include <dynamo/dynamo.hpp>
#include <dynamo/gui/dynamo_inspector.hpp>

class Sandbox : public app::Application{

private:
    flecs::world world {};
    dynamo_gui::Inspector debug {world};

public:
    Sandbox() :
        Application(1280,720,"Sandbox", "Sandbox")
    {
        auto& sim = dynamo::module_ref<dynamo::Simulation>(world.import<dynamo::Simulation>());

        auto core = world.get<dynamo::module::Core>();

        int n = 10;
        for(int i = 0; i<n; i++){
            auto e = world.entity(fmt::format("Agent {}", i).c_str()).is_a(core->Agent);

            e.each([](flecs::id &id) {
                if (id.is_pair()) {
                    dynamo::logger(id)->info("( {} - {} )", id.relation().name().c_str(), id.object().name().c_str());
                } else {
                    dynamo::logger(id)->info("( {} )", id.object().name().c_str());
                }
            });
        }

        world.entity("Radio").is_a(core->Artefact);
    }

private:
    void on_update() override{
        debug.show();
        if(debug.is_enabled){
            world.progress();
        }
    }
};

int main(){
    Sandbox sandbox;
    sandbox.run();
}