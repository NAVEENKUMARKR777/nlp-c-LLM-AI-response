cmake_minimum_required(VERSION 3.15)
project(llm_console)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(llm_console 
    src/main.cpp
    src/llm_client.cpp
)

# Include directories
target_include_directories(llm_console PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Windows specific settings
if(WIN32)
    target_link_libraries(llm_console PRIVATE 
        winhttp
    )
endif()
