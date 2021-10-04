#include <dynamo/modules/core.hpp>

namespace dynamo{
    singleton::Logger::Logger(){
        logger->set_level(spdlog::level::trace);
        logger->set_pattern("[%10n] %^(%8l)%$ %v");
        logger->info("Dynamo launching ...");
    }

    std::shared_ptr<spdlog::logger> logger(flecs::entity e){
        return e.world().get<singleton::Logger>()->logger;
    }

    std::shared_ptr<spdlog::logger> logger(flecs::id id){
        return id.world().get<singleton::Logger>()->logger;
    }

    std::shared_ptr<spdlog::logger> logger(flecs::world& world){
        return world.get<singleton::Logger>()->logger;
    }

    void info_module_header(const std::shared_ptr<spdlog::logger>& logger, const char *name) {
        logger->info("|{0:-^{2}}|", "", fmt::format("Module : {}", name), 75);
        logger->info("|{1: ^{2}}|", "", fmt::format("Module : {}", name), 75);
        logger->info("|{0:-^{2}}|", "", fmt::format("Module : {}", name), 75);
    }

    module::Core::Core(flecs::world &world) {
        world.module<Core>();
        world.add<singleton::Logger>();

        info_module_header(logger(world), "Core");

        // ========== Log ==========

        world.system<EcsComponent>("OnAdd_Component_Log")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, EcsComponent& component) {
                    logger(e)->info("Registering {:>9} : {}", component.size ? "component" : "tag", e.name());
                });

        // ========== Trigger ==========

        world.system<const type::Event>("OnAdd_Event_AddCurrentFrameTag")
                .kind(flecs::OnAdd)
                .each([](flecs::entity e, const type::Event event) {
                    e.set<component::Tick>({e.world().get_tick()});
                    e.add<tag::CurrentFrame>();
                });

        // ========== Pipeline ==========

        world.system<component::Decay>("Decay")
                .kind(flecs::PreFrame)
                .each([](flecs::entity e, component::Decay& decay) {
                    if(decay.ttl <= 0.f){
                        e.destruct();
                    }
                    else{
                        decay.ttl -= e.delta_time();
                    }
                });

        world.system<tag::CurrentFrame>("RemoveCurrentFrameTag")
                .kind(flecs::PostFrame)
                .each([](flecs::entity e, tag::CurrentFrame) {
                    e.remove<tag::CurrentFrame>();
                });

        Action = world.prefab("action_prefab")
                .add<type::Action>()
                .add_owned<type::Action>()
                        ;

        Agent = world.prefab("agent_prefab")
                .add<type::Agent>()
                .add_owned<type::Agent>()
                        ;

        Artefact = world.prefab("artefact_prefab")
                .add<type::Artefact>()
                .add_owned<type::Artefact>()
                        ;

        Event = world.prefab("event_prefab")
                .add<type::Event>()
                .add_owned<type::Event>()
                        ;

        Organisation = world.prefab("organisation_prefab")
                .add<type::Organisation>()
                .add_owned<type::Organisation>()
                        ;

        Percept = world.prefab("percept_prefab")
                .add<type::Percept>()
                .add_owned<type::Percept>()
                        ;
    }
}
