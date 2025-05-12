cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


file(GLOB CPP_SOURCE 
    *.cpp
)

add_executable(PBRman ${CPP_SOURCE})

target_link_libraries(PBRman
    glfw
    nvrhi
    nvrhi_d3d12
    d3d12
    dxgi
)

target_include_directories(PBRman PRIVATE
    src
    ${PROJECT_SOURCE_DIR}/external/glfw/include
    ${PROJECT_SOURCE_DIR}/external/nvrhi/include
)