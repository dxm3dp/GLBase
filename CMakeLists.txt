cmake_minimum_required(VERSION 3.23)
project(GLBase)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

set(TARGET_NAME GLBase)

include_directories(
    "${THIRD_PARTY_DIR}/glad/include"
    "${THIRD_PARTY_DIR}/glfw/include"
    "${THIRD_PARTY_DIR}/glm"
    "${THIRD_PARTY_DIR}/assimp/include"
    "${THIRD_PARTY_DIR}/stb/include"
    "${SRC_DIR}"
)

# assimp
set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_ZLIB OFF)

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT FALSE)
set(ASSIMP_BUILD_OBJ_IMPORTER TRUE)
set(ASSIMP_BUILD_GLTF_IMPORTER TRUE)

add_subdirectory(${THIRD_PARTY_DIR}/assimp)

add_executable(${TARGET_NAME}
    "${SRC_DIR}/main.cpp"
    "${THIRD_PARTY_DIR}/glad/src/glad.c"
)

set(LINK_LIBS assimp)

set(LINK_LIBS ${LINK_LIBS}
    "${THIRD_PARTY_DIR}/glfw/lib-mingw-w64/libglfw3.a"
    "${THIRD_PARTY_DIR}/glfw/lib-mingw-w64/libglfw3dll.a"
)

target_link_libraries(${TARGET_NAME} ${LINK_LIBS})

