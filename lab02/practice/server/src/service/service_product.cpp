#include "service_product.hpp"

#include <cstdint>
#include <sstream>

namespace {
std::string FormatNotFoundError(uint32_t id) {
  std::ostringstream oss;
  oss << "Product with id " << id << " is not present";
  return oss.str();
}
} // namespace

oatpp::Object<ProductDto>
ProductService::createProduct(const oatpp::Object<ProductDto> &dto) {
  dto->id = ++ProductsCounter;
  Products.insert({dto->id, dto});
  return dto;
}

oatpp::Object<ProductDto>
ProductService::updateProduct(const oatpp::Int32 &id,
                              const oatpp::Object<ProductDto> &dto) {
  auto it = Products.find(id);
  OATPP_ASSERT_HTTP(it != Products.end(), Status::CODE_404,
                    FormatNotFoundError(id));
  auto &product = it->second;
  if (!dto->name->empty())
    product->name = dto->name;
  if (!dto->description->empty())
    product->description = dto->description;
  return product;
}

oatpp::Object<ProductDto>
ProductService::getProductById(const oatpp::Int32 &id) {
  auto it = Products.find(id);
  OATPP_ASSERT_HTTP(it != Products.end(), Status::CODE_404,
                    FormatNotFoundError(id));
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

oatpp::Object<ProductDto>
ProductService::deleteProductById(const oatpp::Int32 &id) {
  auto it = Products.find(id);
  OATPP_ASSERT_HTTP(it != Products.end(), Status::CODE_404,
                    FormatNotFoundError(id));
  auto product = ProductDto::createShared();
  product = std::move(it->second);
  Products.erase(it);
  return product;
}
