#ifndef DYNAMO_APPLICATION_H
#define DYNAMO_APPLICATION_H

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <spdlog/spdlog.h>

namespace app {

    class Application {
    public:
        explicit Application(int width = 1280, int height = 720, const char * title = "Title", const char * logger_name = "Engine");
        ~Application();
        void run();

    protected:
        virtual void on_update() = 0;

    public:
        std::shared_ptr<spdlog::logger> logger;

    private:
        GLFWwindow*     window;
        bool            show_demo_window = true;
        bool            is_dockspace_open = true;

    };
}

#endif //DYNAMO_APPLICATION_H