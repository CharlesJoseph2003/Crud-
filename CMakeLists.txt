cmake_minimum_required(VERSION 3.10)
project(synth_presets)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

add_executable(synth_presets main.cpp preset_manager.cpp)
target_link_libraries(synth_presets PRIVATE nlohmann_json::nlohmann_json)
