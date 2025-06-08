#pragma once

#include <cstdint>

#include "dto/dto_product.hpp"

// TODO: change " to <>
#include "oatpp/core/Types.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"

#include "util/images.hpp"

class ProductService {
   private:
    typedef oatpp::web::protocol::http::Status Status;
    std::unordered_map<uint32_t, oatpp::Object<ProductDto>> Products;
    uint32_t ProductsCounter = 0;
    util::IImageSaverPtr ImageSaver = util::CreateImageSaver();

   public:
    oatpp::Object<ProductDto> createProduct(const oatpp::Object<ProductDto> &dto);
    oatpp::Object<ProductDto> updateProduct(const oatpp::Int32 &id,
                                            const oatpp::Object<ProductDto> &dto);
    oatpp::Object<ProductDto> getProductById(const oatpp::Int32 &id);
    oatpp::List<oatpp::Object<ProductDto>> getAllProducts();
    oatpp::Object<ProductDto> deleteProductById(const oatpp::Int32 &id);
    oatpp::Object<ProductDto> addProductIcon(const oatpp::Int32 &id, std::string body);
    std::shared_ptr<oatpp::web::protocol::http::outgoing::StreamingBody> getProductIcon(const oatpp::Int32 &id);
};
