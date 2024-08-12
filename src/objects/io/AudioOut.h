/// AudioOut.h

#pragma once

#include "../Object.h"
#include "miniaudio.h"
#include "../../widgets/Connector.h"

#include <memory>
#include <optional>
#include <string>

namespace flap {
    class AudioOut : public Object {
    public:
        AudioOut(ma_device_info device, int channels, std::optional<std::string> name = std::nullopt) : Object(), _device(device), _channels(channels), _name(name) {}
        void initialize() override;
        void render() override;
    private:
        ma_device_info _device;
        int _channels;
        Connector _input;
        Connector _inputL;
        Connector _inputR;
        std::optional<std::string> _name;
    };
}