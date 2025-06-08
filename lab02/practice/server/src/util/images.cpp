#include "images.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace util {

namespace {

class TImageSaver : public IImageSaver {
    const TPath ImagesDir;

   public:
    TImageSaver(const TPath &imagesDir) : ImagesDir(imagesDir) {}

    ~TImageSaver() override = default;

    TPath SaveImage(std::string imageBytes, uint32_t productId) override {
        const auto filePath = GetImagePath(productId);
        auto file = std::ofstream(filePath, std::ios::binary | std::ios::out);
        file.write(imageBytes.data(), imageBytes.size());
        return filePath;
    }

    TPath GetImagePath(uint32_t productId) override {
        // TODO: filesystem::path
        const auto filePath = ImagesDir + "/" + std::to_string(productId);
        return filePath;
    }
};

}  // namespace

IImageSaverPtr CreateImageSaver() {
    TPath imagesDir = std::tmpnam(NULL);

    if (!std::filesystem::create_directory(imagesDir)) {
        throw std::runtime_error("Cannot create directory for images");
    }
    // TODO: add logging
    std::cout << std::endl << "Images directory path: " << imagesDir << std::endl;
    return std::make_unique<TImageSaver>(imagesDir);
}
}  // namespace util