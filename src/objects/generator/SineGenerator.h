/// SineGenerator.h

#pragma once

#include "../Object.h"
#include "../../MainApplicationSettings.h"
#include "../../widgets/Connector.h"

#include <memory>

namespace flap {
    class SineGenerator : public Object {
        public:
            SineGenerator() : Object() {}
            void initialize() override;
            void render() override;
        private:
            Connector _input;
            Connector _output;
    };
}