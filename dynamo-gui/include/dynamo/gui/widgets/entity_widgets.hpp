#ifndef DYNAMO_ENTITY_WIDGETS_HPP
#define DYNAMO_ENTITY_WIDGETS_HPP

#include <flecs.h>

namespace dynamo_gui::widget {
    void show_action_widget(flecs::entity& entity);
    void show_agent_widget(flecs::entity& entity);
    void show_artefact_widget(flecs::entity& entity);
    void show_percept_widget(flecs::entity& entity);
    void show_organisation_widget(flecs::entity& entity);
}

#endif //DYNAMO_ENTITY_WIDGETS_HPP
