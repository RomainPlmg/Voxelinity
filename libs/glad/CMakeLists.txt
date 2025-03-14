project(glad)

set(GLAD_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC
        ${GLAD_HEADER_DIR}
)