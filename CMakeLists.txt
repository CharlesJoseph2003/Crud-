itcmake_minimum_required(VERSION 3.10)
project(synth_presets)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add ws2_32 library for Windows sockets
if(WIN32)
    link_libraries(ws2_32)
endif()

# Include FetchContent for downloading dependencies
include(FetchContent)

# Fetch nlohmann_json
FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

add_executable(synth_presets 
    main.cpp
    preset_manager.cpp
)

# Link libraries
target_link_libraries(synth_presets PRIVATE nlohmann_json::nlohmann_json)
