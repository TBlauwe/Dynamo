#ifndef DYNAMO_APP_HPP
#define DYNAMO_APP_HPP

#include <application/application.h>
#include <spdlog/spdlog.h>
#include <dynamo/dynamo.hpp>
#include <dynamo/gui/debug.hpp>

class Sandbox : public app::Application{
public:
    Sandbox();

private:
    dynamo::Simulation sim;
    dynamo::gui::MainWindow debug;

private:
    void on_update() override;
};

#endif //DYNAMO_APP_HPP