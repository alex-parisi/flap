/// MainApplication.cpp

#include "MainApplication.h"
#include "objects/basic/Gain.h"
#include "objects/generator/SineGenerator.h"
#include "objects/dynamic/Envelope.h"
#include "ConnectionService.h"
#include "managers/ObjectManager.h"
#include "MainApplicationSettings.h"
#include "objects/io/Probe.h"
#include "objects/io/KeyboardSimulator.h"
#include "KeyboardService.h"
#include "ShortcutService.h"
#include "objects/effects/Chorus.h"
#include "objects/effects/Flanger.h"
#include "objects/effects/Phaser.h"
#include "objects/synths/FlapSynth.h"
#include "objects/adaptive/EchoCanceller.h"

#include <iostream>
#include <cstring>
#include <chrono>

void flipShowConnections() {
    ConnectionService::getInstance().setShowConnections(!ConnectionService::getInstance().getShowConnections());
}

void makeGainObject() {
    auto gain = std::make_unique<flap::Gain>("Gain" + std::to_string(flap::ObjectManager::getInstance().gainCounter++));
    gain->initialize();
    {
        std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
        flap::GraphManager::getInstance().addObject(gain->getAudioObjects());
    }
    flap::ObjectManager::getInstance().objects.push_back(std::move(gain));
}

void makeEnvelopeObject() {
    auto envelope = std::make_unique<flap::Envelope>("Envelope" + std::to_string(flap::ObjectManager::getInstance().envelopeCounter++));
    envelope->initialize();
    {
        std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
        flap::GraphManager::getInstance().addObject(envelope->getAudioObjects());
    }
    flap::ObjectManager::getInstance().objects.push_back(std::move(envelope));
}

void makeSineGeneratorObject() {
    auto sine = std::make_unique<flap::SineGenerator>("SineGenerator" + std::to_string(flap::ObjectManager::getInstance().sineCounter++));
    sine->initialize();
    {
        std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
        flap::GraphManager::getInstance().addObject(sine->getAudioObjects());
    }
    flap::ObjectManager::getInstance().objects.push_back(std::move(sine));
}

void makeProbeObject() {
    auto probe = std::make_unique<flap::Probe>("Probe" + std::to_string(flap::ObjectManager::getInstance().probeCounter++));
    probe->initialize();
    {
        std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
        flap::GraphManager::getInstance().addObject(probe->getAudioObjects());
    }
    flap::ObjectManager::getInstance().objects.push_back(std::move(probe));
}

void makeKeyboardSimulatorObject() {
    /// Pull out the port number from the string
    auto keyboardSim = flap::MidiManager::getInstance().createSimulator();
    /// Pull out the midiIn object and add it to the graph
    {
        std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
        flap::GraphManager::getInstance().addObject(keyboardSim->getAudioObjects());
    }
    flap::ObjectManager::getInstance().objects.push_back(std::move(keyboardSim));
}

void makeAudioOutObject(ma_device_info p) {
    /// Convert the format to a ma_format
    ma_format format;
    if (flap::MainApplicationSettingsManager::getInstance().settings.format == "f32") {
        format = ma_format_f32;
    } else if (flap::MainApplicationSettingsManager::getInstance().settings.format == "s16") {
        format = ma_format_s16;
    } else if (flap::MainApplicationSettingsManager::getInstance().settings.format == "s24") {
        format = ma_format_s24;
    } else if (flap::MainApplicationSettingsManager::getInstance().settings.format == "s32") {
        format = ma_format_s32;
    } else {
        format = ma_format_f32;
    }
    /// Pull out the device info and add it to the graph
    auto audioOut = flap::AudioManager::getInstance().openPlaybackDevice(p, format, 2);
    /// If the device was opened successfully, add it to the graph
    if (audioOut.has_value()) {
        {
            std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
            flap::GraphManager::getInstance().addObject(audioOut.value()->getAudioObjects());
        }
        /// Set the graph signal and mutex - this lets the two talk to each other
        /// TODO: Does this need to be protected by the mutex?
        auto a = audioOut.value()->getAudioObjects()[0];
        auto sink = static_cast<dibiff::sink::GraphSink*>(a);
        flap::GraphManager::getInstance().addOutputGraphSignal(&sink->cv);
        flap::GraphManager::getInstance().addOutputGraphMutex(&sink->cv_mtx);
        flap::ObjectManager::getInstance().sharedObjects.push_back(audioOut.value());
    }
}

void makeAudioInObject(ma_device_info p) {
    /// Convert the format to a ma_format
    ma_format format;
    if (flap::MainApplicationSettingsManager::getInstance().settings.format == "f32") {
        format = ma_format_f32;
    } else if (flap::MainApplicationSettingsManager::getInstance().settings.format == "s16") {
        format = ma_format_s16;
    } else if (flap::MainApplicationSettingsManager::getInstance().settings.format == "s24") {
        format = ma_format_s24;
    } else if (flap::MainApplicationSettingsManager::getInstance().settings.format == "s32") {
        format = ma_format_s32;
    } else {
        format = ma_format_f32;
    }
    /// Pull out the device info and add it to the graph
    auto audioIn = flap::AudioManager::getInstance().openCaptureDevice(p, format, 2);
    /// If the device was opened successfully, add it to the graph
    if (audioIn.has_value()) {
        {
            std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
            flap::GraphManager::getInstance().addObject(audioIn.value()->getAudioObjects());
        }
        /// Set the graph signal and mutex - this lets the two talk to each other
        /// TODO: Does this need to be protected by the mutex?
        auto a = audioIn.value()->getAudioObjects()[0];
        auto source = static_cast<dibiff::source::GraphSource*>(a);
        flap::GraphManager::getInstance().addOutputGraphSignal(&source->cv);
        flap::GraphManager::getInstance().addOutputGraphMutex(&source->cv_mtx);
        flap::ObjectManager::getInstance().sharedObjects.push_back(audioIn.value());
    }
}

void makeMidiInObject(std::string i) {
    /// Pull out the port number from the string
    int port = std::stoi(i.substr(1, i.find("]") - 1));
    auto midiInObj = flap::MidiManager::getInstance().openInputPort(port);
    /// If the port was opened successfully, add it to the graph
    if (midiInObj) {
        /// Pull out the midiIn object and add it to the graph
        {
            std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
            flap::GraphManager::getInstance().addObject(midiInObj->getAudioObjects());
        }
        flap::ObjectManager::getInstance().objects.push_back(std::move(midiInObj));
    }
}

void makeChorusObject() {
    auto chorus = std::make_unique<flap::Chorus>("Chorus" + std::to_string(flap::ObjectManager::getInstance().chorusCounter++));
    chorus->initialize();
    {
        std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
        flap::GraphManager::getInstance().addObject(chorus->getAudioObjects());
    }
    flap::ObjectManager::getInstance().objects.push_back(std::move(chorus));
}

void makeFlangerObject() {
    auto flanger = std::make_unique<flap::Flanger>("Flanger" + std::to_string(flap::ObjectManager::getInstance().flangerCounter++));
    flanger->initialize();
    {
        std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
        flap::GraphManager::getInstance().addObject(flanger->getAudioObjects());
    }
    flap::ObjectManager::getInstance().objects.push_back(std::move(flanger));
}

void makePhaserObject() {
    auto phaser = std::make_unique<flap::Phaser>("Phaser" + std::to_string(flap::ObjectManager::getInstance().phaserCounter++));
    phaser->initialize();
    {
        std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
        flap::GraphManager::getInstance().addObject(phaser->getAudioObjects());
    }
    flap::ObjectManager::getInstance().objects.push_back(std::move(phaser));
}

void makeFlapSynthObject() {
    auto start = std::chrono::high_resolution_clock::now();
    auto flapSynth = std::make_unique<flap::FlapSynth>("FlapSynth" + std::to_string(flap::ObjectManager::getInstance().flapSynthCounter++));
    flapSynth->initialize();
    {
        std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
        flap::GraphManager::getInstance().addObject(flapSynth->getAudioObjects());
    }
    flap::ObjectManager::getInstance().objects.push_back(std::move(flapSynth));
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // std::cout << "FlapSynth object created in " << duration.count() << "ms\n";
}

void makeEchoCancellerObject() {
    auto aec = std::make_unique<flap::EchoCanceller>("EchoCanceller" + std::to_string(flap::ObjectManager::getInstance().echoCancellerCounter++));
    aec->initialize();
    {
        std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
        flap::GraphManager::getInstance().addObject(aec->getAudioObjects());
    }
    flap::ObjectManager::getInstance().objects.push_back(std::move(aec));
}

void buildMenuEntry(std::string title, std::function<void()> action, std::optional<std::string> shortcut = std::nullopt, std::optional<bool*> selected = std::nullopt, bool enabled = true) {
    if (ImGui::MenuItem(title.c_str(), shortcut.has_value() ? shortcut.value().c_str() : NULL, selected.has_value() ? selected.value() : NULL, enabled)) {
        action();
    }
}

bool flap::MainApplication::initialize() {
    // Initialize GLFW
    glfwSetErrorCallback(_glfwErrorCallback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }
    // Set GLFW context version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create a GLFW window
    _window = glfwCreateWindow(1280, 720, "FLAP", NULL, NULL);
    if (!_window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);
    // Load OpenGL functions using glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize OpenGL context\n";
        return false;
    }
    // Set up ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    // Initialize ImGui backends
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    // Initialize managers
    if (!flap::AudioManager::getInstance().initialize()) {
        std::cerr << "Failed to initialize AudioManager\n";
        return false;
    }
    if (!flap::GraphManager::getInstance().initialize()) {
        std::cerr << "Failed to initialize GraphManager\n";
        return false;
    }
    if (!flap::MidiManager::getInstance().initialize()) {
        std::cerr << "Failed to initialize MidiManager\n";
        return false;
    }

    /// Add shortcuts
    flap::ShortcutService::getInstance().shortcuts.push_back({{ImGuiKey_LeftSuper, ImGuiKey_C}, [](){ flipShowConnections(); }});
    flap::ShortcutService::getInstance().shortcuts.push_back({{ImGuiKey_LeftSuper, ImGuiKey_P}, [](){ makeProbeObject(); }});
    flap::ShortcutService::getInstance().shortcuts.push_back({{ImGuiKey_LeftSuper, ImGuiKey_K}, [](){ makeKeyboardSimulatorObject(); }});
    return true;
}

void flap::MainApplication::run() {
    // Manager Threads
    flap::GraphManager::getInstance().run();
    flap::AudioManager::getInstance().run();
    flap::MidiManager::getInstance().run();
    // Main render loop / UI Thread
    while (!glfwWindowShouldClose(_window)) {
        _render();
    }
    // Stop the application when closed
    std::cout << "Quitting...\n";
    flap::AudioManager::getInstance().stop();
    flap::GraphManager::getInstance().stop();
    flap::MidiManager::getInstance().stop();
}

void flap::MainApplication::cleanup() {
    flap::AudioManager::getInstance().cleanup();
    flap::GraphManager::getInstance().cleanup();
    flap::MidiManager::getInstance().cleanup();
    if (_window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(_window);
        glfwTerminate();
    }
}

void flap::MainApplication::_render() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(currentTime.time_since_epoch()).count();
    if ((elapsedTime - _lastRenderTime) >= _renderInterval) {
        _lastRenderTime = elapsedTime;
        /// Poll for input events
        glfwPollEvents();
        /// Update simulators
        // flap::MidiManager::getInstance().stepSimulators();
        /// Draw all ImGui elements
        int display_w, display_h;
        glfwGetFramebufferSize(_window, &display_w, &display_h);
        _renderImGui(display_w, display_h);
        /// Render OpenGL draw data
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(_window);
        // Sleep for the render interval
        std::this_thread::sleep_for(std::chrono::duration<double>(_renderInterval));
    }
}

void flap::MainApplication::_renderImGui(int screenWidth, int screenHeight) {
    /// Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    /// Set next window size to cover the entire screen
    ImGui::SetNextWindowSize(ImVec2((float)screenWidth, (float)screenHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    /// Create a main window with no decorations (title bar, resize handles, etc.)
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Main Window", NULL, windowFlags);
    _renderToolbar();

    /// Delete objects that have been marked for deletion
    ObjectManager::getInstance().objects.erase(std::remove_if(ObjectManager::getInstance().objects.begin(), ObjectManager::getInstance().objects.end(), [](std::unique_ptr<Object>& obj) {
        return obj->shouldDelete;
    }), ObjectManager::getInstance().objects.end());
    ObjectManager::getInstance().sharedObjects.erase(std::remove_if(ObjectManager::getInstance().sharedObjects.begin(), ObjectManager::getInstance().sharedObjects.end(), [](std::shared_ptr<Object>& obj) {
        return obj->shouldDelete;
    }), ObjectManager::getInstance().sharedObjects.end());
    
    /// Render all other ImGui UI elements here
    for (auto& object : ObjectManager::getInstance().objects) {
        object->render();
    }
    for (auto& sharedObject : ObjectManager::getInstance().sharedObjects) {
        sharedObject->render();
    }

    /// Render connections
    ConnectionService::getInstance().renderConnections();

    /// End the main window
    ImGui::End();
    /// Rendering
    ImGui::Render();
}

void flap::MainApplication::_renderToolbar() {
    if (ImGui::BeginMenuBar()) {
        _renderFileMenu(*_window);
        _renderEditMenu();
        _renderGraphMenu();
        ImGui::EndMenuBar();
        flap::KeyboardService::getInstance().update();
        flap::ShortcutService::getInstance().update();
    }
}

void flap::MainApplication::_renderFileMenu(GLFWwindow& window) {
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open")) {
            /// TODO: Handle open action
        }
        if (ImGui::MenuItem("Save")) {
            /// TODO: Handle save action
        }
        if (ImGui::BeginMenu("Settings")) {
            _renderSettingsSubMenu();
        }
        if (ImGui::MenuItem("Quit")) {
            glfwSetWindowShouldClose(&window, GLFW_TRUE);
        }
        ImGui::EndMenu();
    }
}

void flap::MainApplication::_renderSettingsSubMenu() {
    if (ImGui::BeginMenu("Sample Rate")) {
        for (auto& rate : flap::MainApplicationSettingsManager::getInstance().settings.supportedSampleRates) {
            bool selected = rate == flap::MainApplicationSettingsManager::getInstance().settings.sampleRate;
            if (ImGui::MenuItem(std::to_string(rate).c_str(), NULL, &selected)) {
                /// TODO: Set the sample rate
                flap::MainApplicationSettingsManager::getInstance().settings.sampleRate = rate;
            }
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Block Size")) {
        for (auto& size : flap::MainApplicationSettingsManager::getInstance().settings.supportedBlockSizes) {
            bool selected = size == flap::MainApplicationSettingsManager::getInstance().settings.blockSize;
            if (ImGui::MenuItem(std::to_string(size).c_str(), NULL, &selected)) {
                /// TODO: Set the block size
                flap::MainApplicationSettingsManager::getInstance().settings.blockSize = size;
            }
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Format")) {
        for (auto& format : flap::MainApplicationSettingsManager::getInstance().settings.supportedFormats) {
            bool selected = format == flap::MainApplicationSettingsManager::getInstance().settings.format;
            if (ImGui::MenuItem(format.c_str(), NULL, &selected)) {
                /// TODO: Set the format
                flap::MainApplicationSettingsManager::getInstance().settings.format = format;
            }
        }
        ImGui::EndMenu();
    }
    ImGui::EndMenu();    
}

void flap::MainApplication::_renderEditMenu() {
    if (ImGui::BeginMenu("Edit")) {
        if (ImGui::MenuItem("Show Connections", "^ + C", ConnectionService::getInstance().getShowConnections())) {
            flipShowConnections();
        }
        if (ImGui::MenuItem("Undo")) {
            /// TODO: Handle undo action
        }
        if (ImGui::MenuItem("Redo")) {
            /// TODO: Handle redo action
        }
        ImGui::EndMenu();
    }
}

void flap::MainApplication::_renderGraphMenu() {
    if (ImGui::BeginMenu("Graph")) {
        if (ImGui::BeginMenu("Add Object")) {
            if (ImGui::BeginMenu("Synth")) {
                buildMenuEntry("FLAP Synth", makeFlapSynthObject);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Adaptive")) {
                buildMenuEntry("Echo Canceller", makeEchoCancellerObject);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Basic")) {
                buildMenuEntry("Gain", makeGainObject);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Dynamic")) {
                buildMenuEntry("Envelope", makeEnvelopeObject);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Effect")) {
                buildMenuEntry("Chorus", makeChorusObject);
                buildMenuEntry("Flanger", makeFlangerObject);
                buildMenuEntry("Phaser", makePhaserObject);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Generator")) {
                buildMenuEntry("Sine Generator", makeSineGeneratorObject);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Add Audio I/O")) {
            buildMenuEntry("Probe", makeProbeObject, "^ + P");
            if (ImGui::BeginMenu("Input")) {
                for (auto& c : flap::AudioManager::getInstance().getCaptureDevices()) {
                    bool selected = false;
                    for (auto& d : flap::AudioManager::getInstance().getOpenedCaptureDevices()) {
                        if (std::strcmp(c.name, d.name) == 0) {
                            selected = true;
                            break;
                        }
                    }
                    buildMenuEntry(c.name, [=](){ makeAudioInObject(c); }, std::nullopt, &selected, !selected);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Output")) {
                for (auto& p : flap::AudioManager::getInstance().getPlaybackDevices()) {
                    bool selected = false;
                    for (auto& d : flap::AudioManager::getInstance().getOpenedPlaybackDevices()) {
                        if (std::strcmp(p.name, d.name) == 0) {
                            selected = true;
                            break;
                        }
                    }
                    buildMenuEntry(p.name, [=](){ makeAudioOutObject(p); }, std::nullopt, &selected, !selected);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Add MIDI I/O")) {
            if (ImGui::BeginMenu("MIDI Input")) {
                buildMenuEntry("Keyboard Simulator", makeKeyboardSimulatorObject, "^ + K");
                for (auto& i : flap::MidiManager::getInstance().getInputPortNames()) {
                    if (ImGui::MenuItem(i.c_str())) {
                        makeMidiInObject(i);
                    }
                    buildMenuEntry(i, [=](){ makeMidiInObject(i); });
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("MIDI Output")) {
                for (auto& o : flap::MidiManager::getInstance().getOutputPortNames()) {
                    if (ImGui::MenuItem(o.c_str())) {
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}