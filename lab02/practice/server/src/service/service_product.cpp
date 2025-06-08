#include "service_product.hpp"

#include <cstdint>
#include <sstream>

#include "oatpp/core/data/resource/File.hpp"

namespace {
std::string FormatNotFoundError(uint32_t id) {
    std::ostringstream oss;
    oss << "Product with id " << id << " is not present";
    return oss.str();
}
}  // namespace

oatpp::Object<ProductDto> ProductService::createProduct(const oatpp::Object<ProductDto> &dto) {
    dto->id = ++ProductsCounter;
    Products.insert({dto->id, dto});
    return dto;
}

oatpp::Object<ProductDto> ProductService::updateProduct(const oatpp::Int32 &id,
                                                        const oatpp::Object<ProductDto> &dto) {
    auto it = Products.find(id);
    OATPP_ASSERT_HTTP(it != Products.end(), Status::CODE_404, FormatNotFoundError(id));

    auto &product = it->second;

    if (!dto->name->empty()) product->name = std::move(dto->name);
    if (!dto->description->empty()) product->description = std::move(dto->description);
    return product;
}

oatpp::Object<ProductDto> ProductService::addProductIcon(const oatpp::Int32 &id, std::string body) {
    auto it = Products.find(id);
    OATPP_ASSERT_HTTP(it != Products.end(), Status::CODE_404, FormatNotFoundError(id));
    auto &product = it->second;

    OATPP_ASSERT_HTTP(product->image != "null", Status::CODE_409,
                      "Product " + std::to_string(id) + " already has an icon");

    product->image = ImageSaver->SaveImage(std::move(body), id);
    return product;
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::StreamingBody> ProductService::getProductIcon(
    const oatpp::Int32 &id) {
    auto it = Products.find(id);
    OATPP_ASSERT_HTTP(it != Products.end(), Status::CODE_404, FormatNotFoundError(id));

    auto &product = it->second;

    OATPP_ASSERT_HTTP(product->image != "null", Status::CODE_404,
                      "Product " + std::to_string(id) + " doesn't have an icon");

    const auto imagePath = ImageSaver->GetImagePath(id);
    return std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>(
        oatpp::data::resource::File(imagePath).openInputStream());
}

oatpp::Object<ProductDto> ProductService::getProductById(const oatpp::Int32 &id) {
    auto it = Products.find(id);
    OATPP_ASSERT_HTTP(it != Products.end(), Status::CODE_404, FormatNotFoundError(id));
    auto product = ProductDto::createShared();
    product = it->second;
    return product;
}

oatpp::List<oatpp::Object<ProductDto>> ProductService::getAllProducts() {
    auto result = oatpp::List<oatpp::Object<ProductDto>>::createShared();
    for (const auto &[key, value] : Products) {
        std::ignore = key;
        result->push_back(value);
    }
    return result;
}

oatpp::Object<ProductDto> ProductService::deleteProductById(const oatpp::Int32 &id) {
    auto it = Products.find(id);
    OATPP_ASSERT_HTTP(it != Products.end(), Status::CODE_404, FormatNotFoundError(id));
    auto product = ProductDto::createShared();
    product = std::move(it->second);
    Products.erase(it);
    return product;
}
