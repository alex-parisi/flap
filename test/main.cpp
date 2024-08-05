/// main.cpp

#include "../src/MainApplication.h"
#include "../src/MainApplicationSettings.h"

int main() {
    auto settings = std::make_shared<flap::MainApplicationSettings>();
    flap::MainApplication app(settings);
    if (!app.initialize()) {
        return -1;
    }
    app.run();    
    app.cleanup();
    return 0;
}