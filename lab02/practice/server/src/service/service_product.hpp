#pragma once

#include "dto/dto_product.hpp"

#include "oatpp/core/Types.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include <cstdint>

class ProductService {
private:
  typedef oatpp::web::protocol::http::Status Status;
  std::unordered_map<uint32_t, oatpp::Object<ProductDto>> Products;
  uint32_t ProductsCounter = 0;

public:
  oatpp::Object<ProductDto> createProduct(const oatpp::Object<ProductDto> &dto);
  oatpp::Object<ProductDto> updateProduct(const oatpp::Int32 &id,
                                          const oatpp::Object<ProductDto> &dto);
  oatpp::Object<ProductDto> getProductById(const oatpp::Int32 &id);
  oatpp::List<oatpp::Object<ProductDto>> getAllProducts();
  oatpp::Object<ProductDto> deleteProductById(const oatpp::Int32 &id);
};
