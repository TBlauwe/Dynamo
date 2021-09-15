#ifndef DYNAMO_APP_HPP
#define DYNAMO_APP_HPP

#include <application/application.h>
#include <spdlog/spdlog.h>
#include <dynamo/dynamo.hpp>
#include <dynamo/gui/debug.hpp>


class Sandbox : public app::Application{
public:
    Sandbox():
            Application(1280,720,"Sandbox", "Sandbox"),
            sim()
    {

    }

private:
    dynamo::Simulation sim;

private:
    void on_update() override {
        dynamo::gui::show(sim);
    }
};

#endif //DYNAMO_APP_HPP