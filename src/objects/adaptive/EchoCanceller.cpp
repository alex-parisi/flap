/// EchoCanceller.cpp

#include "EchoCanceller.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui-knobs.h"

#include "../../ConnectionService.h"

void flap::EchoCanceller::initialize() {
    auto aecObject = dibiff::adaptive::AcousticEchoCanceller::create(_filterLength, _stepSize);
    _audioObjects.push_back(std::move(aecObject));
    _input = Connector(_audioObjects[0].get()->getInput(0), _audioObjects[0].get());
    _reference = Connector(_audioObjects[0].get()->getInput(1), _audioObjects[0].get());
    _output = Connector(_audioObjects[0].get()->getOutput(), _audioObjects[0].get(), true);
}

void flap::EchoCanceller::render() {
        std::string title;
    if (_name.has_value()) {
        title = *_name;
    } else {
        title = "EchoCanceller";
    }
    ImGui::Begin(title.c_str(), _isOpen);
    ImGui::SeparatorText("Connections");
    _input.render("In");
    ImGui::SameLine();
    _reference.render("Ref");
    ImGui::SameLine();
    _output.render("Out");
    ImGui::SeparatorText("EchoCanceller Parameters");
    ImGui::End();
    _isOpen.check();
}