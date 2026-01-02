cmake_minimum_required(VERSION 3.20)
project(VisualEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VCPKG_APPLOCAL_DEPS ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(Angelscript CONFIG REQUIRED)

file(GLOB ENGINE_SOURCES
    CONFIGURE_DEPENDS
    "*.cpp"
    "*.h"
    "source/**/*.cpp"
    "source/**/*.h"
)


add_executable(${PROJECT_NAME} ${ENGINE_SOURCES} "source/components/component.h" "source/components/mesh_component.h" "source/components/camera_component.h" "source/components/script_component.h" "source/components/transform_component.h")

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad::glad
    glm::glm
    assimp::assimp
    imgui::imgui
    imguizmo::imguizmo
    Angelscript::angelscript
)

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(ASSET_FILES
    "test_model.glb"
    "source/renderer/shaders/default.vert"
    "source/renderer/shaders/default.frag"
    "thirdparty/stb_image.h"
)

foreach(FILE ${ASSET_FILES})
    configure_file(${FILE} ${CMAKE_CURRENT_BINARY_DIR}/${FILE} COPYONLY)
endforeach()