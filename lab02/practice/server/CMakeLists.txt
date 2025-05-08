set(SOURCES
    src/service/service_product.cpp
    src/main.cpp
)

find_package(oatpp REQUIRED)

add_executable(server ${SOURCES})

target_include_directories(server PRIVATE
  src/
)

target_link_libraries(server 
  PUBLIC oatpp)

