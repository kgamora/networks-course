#pragma once

#include <memory>
#include <string>

namespace util {

using TPath = std::string;

class IImageSaver {
   public:
    virtual ~IImageSaver() = default;
    virtual TPath SaveImage(std::string, uint32_t) = 0;
    virtual TPath GetImagePath(uint32_t) = 0;
};

using IImageSaverPtr = std::unique_ptr<IImageSaver>;

IImageSaverPtr CreateImageSaver();
}  // namespace util