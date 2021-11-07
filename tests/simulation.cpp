#include <doctest/doctest.h>
#include <dynamo/simulation.hpp>

TEST_SUITE_BEGIN("Simulation");

TEST_CASE("Basics") {
    using namespace dynamo;
    auto sim = Simulation();

    SUBCASE("Loaded modules"){
        CHECK(sim.world().has<module::Core>());
    }

    SUBCASE("Running"){
        sim.step();
        CHECK(sim.world().get_tick() == 1);
        sim.step_n(10);
        CHECK(sim.world().get_tick() == 11);
    }

    SUBCASE("Entities") {
        auto empty = sim.world().entity();
        CHECK(empty.name()[0] == '\0');

        const char * name = "Some name";
        auto a = sim.world().entity(name);
        CHECK(strcmp(a.name().c_str(), name)==0);

        auto b = sim.world().entity(name);
        CHECK(strcmp(b.name().c_str(), name)==0);
        CHECK(a == b);
        CHECK(a.id() == b.id());
    }

    SUBCASE("Agents"){
        auto arthur = sim.agent("Arthur").entity();
        CHECK(arthur.has<type::Agent>());
    }

    SUBCASE("Artefacts"){
        auto radio = sim.artefact("Radio").entity();
        CHECK(radio.has<type::Artefact>());
    }

    struct Default{};

    SUBCASE("Percepts"){
        auto arthur = sim.agent("arthur");
        auto radio = sim.artefact("Radio");
        float ttl = 2.0f;

        auto percept = sim.percept<Default>(radio)
                .decay(ttl)
                .perceived_by(arthur)
                .entity();

        CHECK(percept.has<type::Percept>());
        CHECK(percept.has<Default>());
        CHECK(percept.has<component::Decay>());
        CHECK(percept.get<component::Decay>()->ttl);
        CHECK(percept.has<relation::perceive>(radio));
        CHECK(arthur.entity().has<relation::perceive>(percept));

        sim.step(ttl); // To deplete decay cooldown
        sim.step(); // To delete entity
        CHECK(percept.is_alive() == false);
    }

    SUBCASE("Queries"){
        sim.agent("Arthur");
        sim.agent("Arthur");
        sim.agent("Bob");

        int count = 0;
        sim.for_each([&count](flecs::entity entity, type::Agent& agent){
            count++;
        });
        CHECK(count == 2);
    }

    SUBCASE("Taskflow"){
    }
}

TEST_SUITE_END();
