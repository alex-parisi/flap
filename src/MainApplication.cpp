/// MainApplication.cpp

#include "MainApplication.h"
#include <iostream>

bool MainApplication::initialize() {
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
    if (!_audioManager.initialize()) {
        std::cerr << "Failed to initialize AudioManager\n";
        return false;
    }
    if (!_graphManager.initialize()) {
        std::cerr << "Failed to initialize GraphManager\n";
        return false;
    }
    if (!_midiManager.initialize()) {
        std::cerr << "Failed to initialize MidiManager\n";
        return false;
    }
    /// Add the MIDI input to the graph
    _graphManager.addObject(_midiManager.getMidiGraphInput());
    /// Add the GraphSink to the graph
    _graphManager.addObject(_audioManager.getGraphSink());
    /// Set the graph signal and mutex
    _graphManager.setGraphSignal(&_audioManager.getGraphSink()->cv);
    _graphManager.setGraphMutex(&_audioManager.getGraphSink()->cv_mtx);

    return true;
}

void MainApplication::run() {
    // Main loop (running on separate threads)
    _audioManager.run();
    _graphManager.run();
    _midiManager.run();
    // Render loop
    while (!glfwWindowShouldClose(_window)) {
        _render();
    }
    // Stop the application when closed
    std::cout << "Quitting...\n";
    _audioManager.stop();
    _graphManager.stop();
    _midiManager.stop();
}

void MainApplication::cleanup() {
    if (_window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(_window);
        glfwTerminate();
    }
    _audioManager.cleanup();
    _graphManager.cleanup();
    _midiManager.cleanup();
}

void MainApplication::_render() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(currentTime.time_since_epoch()).count();
    if ((elapsedTime - _lastRenderTime) >= _renderInterval) {
        _lastRenderTime = elapsedTime;
        glfwPollEvents();
        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // TODO: Render ImGui UI elements here

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(_window);
    }
}