#ifndef DYNAMO_IMGUI_ADDONS_HPP
#define DYNAMO_IMGUI_ADDONS_HPP

#include <string>

#include <imgui.h>
#include <implot.h>
#include <implot_internal.h>
#include <IconsFontAwesome5.h>
#include <boost/circular_buffer.hpp>

namespace ImGui {
    // Some predefined ImGui colors with variants (RGBA):
    // n = normal variant
    // h = hovered variant
    // h = selected variant
    enum Color {
        BLUE_n = IM_COL32(10, 87, 143, 255),
        BLUE_h = IM_COL32(31, 140, 219, 255),
        BLUE_s = IM_COL32(61, 175, 255, 255),

        ORANGE_n = IM_COL32(143, 74, 0, 255),
        ORANGE_h = IM_COL32(219, 113, 0, 255),
        ORANGE_s = IM_COL32(255, 144, 25, 255),

        GREEN_n = IM_COL32(14, 117, 0, 255),
        GREEN_h = IM_COL32(21, 181, 0, 255),
        GREEN_s = IM_COL32(27, 245, 0, 255),

        RED_n = IM_COL32(117, 25, 0, 255),
        RED_h = IM_COL32(181, 39, 0, 255),
        RED_s = IM_COL32(245, 53, 0, 255),

        GRAY_n = IM_COL32(70, 70, 70, 255),
        GRAY_h = IM_COL32(119, 119, 119, 255),
        GRAY_s = IM_COL32(177, 177, 177, 255),

        WHITE_n = IM_COL32(139, 139, 139, 255),
        WHITE_h = IM_COL32(189, 180, 189, 255),
        WHITE_s = IM_COL32(239, 239, 239, 255)
    };
}

namespace ImGui::Widgets{

    void InputFloatColor(float& value);

    // ImPLOT

    template<typename T>
    struct DescriptivePoint{
        T           data;
        std::string annotation {};
        ImVec4      color {ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive)};

        DescriptivePoint(T data):
                data {data}
        {}

        DescriptivePoint(T data, std::string annotation):
                DescriptivePoint(data),
                annotation {std::move(annotation)}
        {}

        DescriptivePoint(T data, std::string annotation, ImVec4 color):
                DescriptivePoint(data, annotation),
                color {color}
        {}
    };

    template<typename T>
    ImPlotPoint descriptive_point_getter(void* data, int idx){
        const auto* array = (boost::circular_buffer<DescriptivePoint<T>>*)data;
        const DescriptivePoint<T> descriptive_point = array->at(idx);
        if(!descriptive_point.annotation.empty())
            ImPlot::AnnotateClamped(idx, descriptive_point.data, ImVec2(15, -15), descriptive_point.color, descriptive_point.annotation.c_str());
        return {static_cast<double>(idx), static_cast<double>(descriptive_point.data)};
    }

    template<typename T>
    class ScrollingPlot{
    private:
        boost::circular_buffer<DescriptivePoint<T>> buffer;
        const char * title;

    public:
        ScrollingPlot(const char * title, int capacity) : title (title), buffer(capacity) {}

        void push(DescriptivePoint<T> point){
            buffer.push_back(point);
        };

        void plot() {
            buffer.linearize();
            ImPlot::PlotLineG(title, ImGui::Widgets::descriptive_point_getter<T>, &buffer, buffer.size());
        }

        int capacity(){
            return buffer.capacity();
        }
    };

    void HelpMarker(const char* desc);
}

#endif //DYNAMO_IMGUI_ADDONS_HPP
