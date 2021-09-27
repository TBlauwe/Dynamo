//
// Created by Tristan on 24/09/2021.
//

#ifndef DYNAMO_ENTITY_WIDGETS_HPP
#define DYNAMO_ENTITY_WIDGETS_HPP

#include <flecs.h>

namespace dynamo::gui::widgets {
    void show_agent_widget(bool* open, flecs::entity& entity);
    void show_artefact_widget(bool* open, flecs::entity& entity);
    void show_percept_widget(bool* open, flecs::entity& entity);
    void show_organisation_widget(bool* open, flecs::entity& entity);
}

#endif //DYNAMO_ENTITY_WIDGETS_HPP
