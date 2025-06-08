#pragma once

#include "dto/dto_product.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "service/service_product.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class ProductController : public oatpp::web::server::api::ApiController {
    ProductService Service;

   public:
    /**
     * Constructor with object mapper.
     * @param objectMapper - default object mapper used to serialize/deserialize
     * DTOs.
     */
    ProductController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper) {}

    static std::shared_ptr<ProductController> createShared(
        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
        return std::make_shared<ProductController>(objectMapper);
    }

    ENDPOINT("POST", "product", createProduct, BODY_DTO(Object<ProductDto>, productDto)) {
        return createDtoResponse(Status::CODE_200, Service.createProduct(productDto));
    }

    ENDPOINT("POST", "product/{productId}/image", addProductIcon, PATH(Int32, productId),
             REQUEST(std::shared_ptr<IncomingRequest>, request)) {
        auto body = request->readBodyToString();
        return createDtoResponse(Status::CODE_200,
                                 Service.addProductIcon(productId, std::move(body)));
    }

    ENDPOINT("GET", "product/{productId}/image", getProductIcon, PATH(Int32, productId)) {
        return OutgoingResponse::createShared(Status::CODE_200, Service.getProductIcon(productId));
    }

    ENDPOINT("PUT", "product/{productId}", putProduct, PATH(Int32, productId),
             BODY_DTO(Object<ProductDto>, productDto)) {
        return createDtoResponse(Status::CODE_200, Service.updateProduct(productId, productDto));
    }

    ENDPOINT("GET", "product/{productId}", getProductById, PATH(Int32, productId)) {
        return createDtoResponse(Status::CODE_200, Service.getProductById(productId));
    }

    ENDPOINT("GET", "products", getProducts) {
        return createDtoResponse(Status::CODE_200, Service.getAllProducts());
    }

    ENDPOINT("DELETE", "product/{productId}", deleteProduct, PATH(Int32, productId)) {
        return createDtoResponse(Status::CODE_200, Service.deleteProductById(productId));
    }
};

#include OATPP_CODEGEN_END(ApiController)
