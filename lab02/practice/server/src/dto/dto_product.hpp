#pragma once

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  Data Transfer Object. Object containing fields only.
 *  Used in API for serialization/deserialization and validation
 */
class ProductDto : public oatpp::DTO {
public:
  using TPath = String;

  DTO_INIT(ProductDto, DTO)

  DTO_FIELD(Int32, id);
  DTO_FIELD(String, name, "name");
  DTO_FIELD(String, description, "description");
  DTO_FIELD(TPath, image, "icon");
};

#include OATPP_CODEGEN_END(DTO)
