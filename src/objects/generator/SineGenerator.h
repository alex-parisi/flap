/// SineGenerator.h

#pragma once

#include "../Object.h"
#include "../../MainApplicationSettings.h"
#include "../../widgets/Connector.h"

#include <memory>

namespace flap {
    class SineGenerator : public Object {
        public:
            inline SineGenerator(std::optional<std::string> name = std::nullopt) : Object(), _name(name) {}
            void initialize() override;
            void render() override;
        private:
            Connector _input;
            Connector _output;
            std::optional<std::string> _name;
    };
}