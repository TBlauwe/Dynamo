//
// Created by Tristan on 21/09/2021.
//

#ifndef DYNAMO_IMGUI_ADDONS_HPP
#define DYNAMO_IMGUI_ADDONS_HPP

#include <string>
#include <imgui.h>
#include <implot.h>
#include <implot_internal.h>
#include <IconsFontAwesome5.h>
#include <boost/circular_buffer.hpp>

namespace ImGui::Addons{
    template<typename T>
    struct DescriptivePoint{
        bool        show_annotation;
        std::string annotation{};
        T           data;

        explicit DescriptivePoint(T data):
                show_annotation     {false},
                data                {data}
        {}

        DescriptivePoint(bool show_annotation, std::string annotation, T data):
                show_annotation{show_annotation},
                annotation {std::move(annotation)},
                data{data}
        {}
    };

    template<typename T>
    ImPlotPoint descriptive_point_getter(void* data, int idx){
        const auto* array = (boost::circular_buffer<DescriptivePoint<T>>*)data;
        const DescriptivePoint<T> descriptive_point = array->at(idx);
        if(descriptive_point.show_annotation)
            ImPlot::AnnotateClamped(idx, descriptive_point.data, ImVec2(15, -15), ImPlot::GetColormapColor(5), descriptive_point.annotation.c_str());
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
            ImPlot::PlotLineG(title, ImGui::Addons::descriptive_point_getter<T>, &buffer, buffer.size());
        }

        int capacity(){
            return buffer.capacity();
        }
    };

    void HelpMarker(const char* desc);
}

#endif //DYNAMO_IMGUI_ADDONS_HPP
