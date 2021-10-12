#include <doctest/doctest.h>
#include <dynamo/dynamo.hpp>

TEST_SUITE_BEGIN("Simulation");

TEST_CASE("Basics") {
    using namespace dynamo;
    auto sim = Simulation();

    SUBCASE("Loaded modules"){
        CHECK(sim.world().has<module::Core>());
        auto core = sim.world().get<module::Core>();
        CHECK(core->Action.is_valid());
        CHECK(core->Agent.is_valid());
        CHECK(core->Artefact.is_valid());
        CHECK(core->Organisation.is_valid());
        CHECK(core->Percept.is_valid());
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
        auto arthur = sim.agent("Arthur");
        CHECK(arthur.has<type::Agent>());
    }

    SUBCASE("Artefacts"){
        auto radio = sim.artefact("Radio");
        CHECK(radio.has<type::Artefact>());
    }

    SUBCASE("Percepts"){
        auto arthur = sim.agent("arthur");
        auto radio = sim.artefact("Radio");
        float ttl = 2.0f;

        auto percept = sim.percept(radio, ttl);
        CHECK(percept.has<type::Percept>());
        CHECK(percept.has<component::Decay>());

        sim.step(ttl); // To deplete decay cooldown
        sim.step(); // To delete entity
        CHECK(percept.is_alive() == false);
    }

    SUBCASE("Queries"){
        auto core = sim.world().get<module::Core>();
        sim.agent("Arthur");
        sim.agent("Arthur");
        sim.agent("Bob");

        int count = 0;
        core->agents_query.each([&count](flecs::entity entity, type::Agent& agent){
            count++;
        });
        CHECK(count == 3);
    }

}

TEST_SUITE_END();
