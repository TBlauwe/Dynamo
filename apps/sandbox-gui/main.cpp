#include <application/application.h>
#include <spdlog/spdlog.h>
#include <dynamo/dynamo.hpp>
#include <IconsFontAwesome5.h>

class Sandbox : public app::Application{
public:
    Sandbox():
        Application(1280,720,"Sandbox"),
        simu()
    {

    }

private:
    dynamo::Simulation simu;

private:
    void on_update() override {
        ImGui::Text(ICON_FA_PAINT_BRUSH "Bonjour");
    }
};

namespace app
{
    void on_initialization()
    {
        spdlog::error("CHAOS launched");
    }

    void on_shutdown()
    {
        spdlog::error("CHAOS Shutting down");
    }
    namespace ImGuiWindows
    {
        void menu_bar()
        {
        }
    }
}

int main(){
    Sandbox sandbox;
    sandbox.run();
}