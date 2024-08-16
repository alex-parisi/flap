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
    _audioObjects.push_back(aecObject);
    _input = Connector(aecObject->getInput(0), aecObject);
    _reference = Connector(aecObject->getInput(1), aecObject);
    _output = Connector(aecObject->getOutput(), aecObject, true);
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