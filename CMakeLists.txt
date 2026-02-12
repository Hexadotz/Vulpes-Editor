cmake_minimum_required(VERSION 3.28)
project(Vulpes-Editor LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.91.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)
set(IMGUI_SFML_FIND_SFML OFF)

FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG v3.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(ImGui-SFML)

set(LIBS 
    "src/assets/libraries/json.hpp" )

set(SOURCES
    "src/main.cpp" 
    "src/UI/uiManager.cpp" 
    "src/UI/editor/uiObjManager.cpp" "src/UI/editor/uiObjManager.h" 
    "src/object/Object.h" 
    "src/application.cpp" "src/application.h"
    "src/UI/editor/uiEditorToolsManager.cpp" "src/UI/editor/uiEditorToolsManager.h" 
    "src/UI/editor/uiConsoleManager.cpp" "src/UI/editor/uiConsoleManager.h")

add_executable(VulpesEditor ${SOURCES} ${LIBS})
target_compile_features(VulpesEditor PRIVATE cxx_std_17)
target_link_libraries(VulpesEditor PRIVATE SFML::Graphics SFML::Audio ImGui-SFML::ImGui-SFML)
