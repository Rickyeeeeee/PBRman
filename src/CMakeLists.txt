cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)



file(GLOB CPP_SOURCE 
    *.cpp
    core/*.cpp
    RayTracing/*.cpp
    RasterEngine/*.cpp
)

file (GLOB HEADER_SOURCE 
    *.h
    core/*.h
    RayTracing/*.h
    RasterEngine/*.h
)

add_executable(
    PBRman 
    ${CPP_SOURCE}
    ${HEADER_SOURCE}
)
set_target_properties(PBRman PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

target_link_libraries(PBRman
    glfw
    nvrhi
    nvrhi_d3d12
    d3d12
    dxgi
    glm::glm
    imgui
    d3dcompiler
    tinyply
)

target_include_directories(PBRman PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/external/glfw/include
    ${PROJECT_SOURCE_DIR}/external/nvrhi/include
    ${PROJECT_SOURCE_DIR}/external/glm
    ${PROJECT_SOURCE_DIR}/external/imgui
    ${PROJECT_SOURCE_DIR}/external/tinyply/source
)