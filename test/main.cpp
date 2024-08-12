/// main.cpp

#include "../src/MainApplication.h"
#include "../src/MainApplicationSettings.h"

int main() {
    if (!flap::MainApplication::getInstance().initialize()) {
        return -1;
    }
    flap::MainApplication::getInstance().run();    
    flap::MainApplication::getInstance().cleanup();
    return 0;
}