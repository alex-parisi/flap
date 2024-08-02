/// main.cpp

#include "../src/MainApplication.h"

int main() {
    MainApplication app;
    if (!app.initialize()) {
        return -1;
    }
    app.run();    
    app.cleanup();
    return 0;
}