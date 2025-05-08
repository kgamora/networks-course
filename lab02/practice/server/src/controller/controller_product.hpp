#pragma once

#include "service/service_product.hpp"

#include "dto/dto_product.hpp"

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class ProductController : public oatpp::web::server::api::ApiController {

  ProductService Service;

public:
  /**
   * Constructor with object mapper.
   * @param objectMapper - default object mapper used to serialize/deserialize
   * DTOs.
   */
  ProductController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>,
                                    objectMapper))
      : oatpp::web::server::api::ApiController(objectMapper) {}

  static std::shared_ptr<ProductController>
  createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
    return std::make_shared<ProductController>(objectMapper);
  }

  ENDPOINT("POST", "products", createProduct,
           BODY_DTO(Object<ProductDto>, productDto)) {
    return createDtoResponse(Status::CODE_200,
                             Service.createProduct(productDto));
  }

  ENDPOINT("PUT", "products/{productId}", putProduct, PATH(Int32, productId),
           BODY_DTO(Object<ProductDto>, productDto)) {
    productDto->id = productId;
    return createDtoResponse(Status::CODE_200,
                             Service.updateProduct(productId, productDto));
  }

  ENDPOINT("GET", "products/{productId}", getProductById,
           PATH(Int32, productId)) {
    return createDtoResponse(Status::CODE_200,
                             Service.getProductById(productId));
  }

  ENDPOINT("GET", "products", getProducts) {
    return createDtoResponse(Status::CODE_200, Service.getAllProducts());
  }

  ENDPOINT("DELETE", "products/{productId}", deleteProduct,
           PATH(Int32, productId)) {
    return createDtoResponse(Status::CODE_200,
                             Service.deleteProductById(productId));
  }
};

#include OATPP_CODEGEN_END(ApiController)
