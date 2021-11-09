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

    struct PrivateValue {
	    float value{ 100.0f };
    };

    struct SharedValue {
	    float value{ 50.0f };
    };

    struct TagOne {};
    struct TagTwo {};

    SUBCASE("Agents"){
        auto archetype = sim.agent_archetype("Archetype_Basic")
            .set<PrivateValue>({ 100.0f })
            .set_shared<SharedValue>({ 50.f })
            .add<TagOne>();

	    auto another_archetype = sim.agent_archetype(archetype, "Archetype_Advanced")
		    .add<TagTwo>();

        auto arthur = sim.agent("Arthur");
        CHECK(arthur.entity().has<type::Agent>());

	    auto bob = sim.agent(archetype, "Bob");
	    auto charlie = sim.agent(another_archetype, "Charlie");
        CHECK(bob.entity().has<TagOne>());
        CHECK(charlie.entity().has<TagOne>());
        CHECK(charlie.entity().has<TagTwo>());
        CHECK(bob.entity().get<PrivateValue>()->value == 100.0f);
        CHECK(bob.entity().get<SharedValue>()->value == 50.0f);

        bob.entity().set<PrivateValue>({ 1.f });
        CHECK(bob.entity().get<PrivateValue>()->value == 1.0f);
        CHECK(charlie.entity().get<PrivateValue>()->value == 100.0f);

        archetype.entity().set<SharedValue>({ 10.f });
        CHECK(bob.entity().get<SharedValue>()->value == 10.0f);
        CHECK(charlie.entity().get<SharedValue>()->value == 10.0f);

        bob.entity().set<SharedValue>({ 1.f }); // When doing this, you add a private copy, so others will not be affected.
        CHECK(bob.entity().get<SharedValue>()->value == 1.0f);
        CHECK(charlie.entity().get<SharedValue>()->value == 10.0f);
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
        CHECK(percept.has<type::Decay>());
        CHECK(percept.get<type::Decay>()->ttl);
        CHECK(percept.has<type::perceive>(radio));
        CHECK(arthur.entity().has<type::perceive>(percept));

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
