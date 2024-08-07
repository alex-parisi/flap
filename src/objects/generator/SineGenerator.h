/// SineGenerator.h

#pragma once

#include "../Object.h"
#include "../../MainApplicationSettings.h"
#include "../../widgets/Connector.h"

#include <memory>

namespace flap {
    class SineGenerator : public Object {
        public:
            SineGenerator(std::shared_ptr<MainApplicationSettings> settings) : Object(), _settings(settings) {}
            void initialize() override;
            void render() override;
        private:
            std::shared_ptr<MainApplicationSettings> _settings;
            Connector _input;
            Connector _output;
    };
}