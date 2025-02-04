cmake_minimum_required(VERSION 3.28.3)

add_compile_definitions(_USE_MATH_DEFINES)

project(2048)

find_package(freetype-gl CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_executable(
    2048
    src/main.cpp
    src/shader.c
    src/mat4.c
    src/FontManager.cpp
    src/Game.cpp
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    target_compile_definitions(2048 PRIVATE M_TILE_MOVE_DURATION=2500000)
    target_compile_definitions(2048 PRIVATE M_TILE_RECOIL_DURATION=1800000)
else()
    target_compile_definitions(2048 PRIVATE M_TILE_MOVE_DURATION=250000000)
    target_compile_definitions(2048 PRIVATE M_TILE_RECOIL_DURATION=180000000)
endif()

target_compile_definitions(2048 PRIVATE M_TARGET=11)
target_compile_definitions(2048 PRIVATE M_GRID_SIZE=4)
target_compile_definitions(2048 PRIVATE GL_SHADERS_PATH="${CMAKE_BINARY_DIR}/shaders/")
target_compile_definitions(2048 PRIVATE GL_FONTS_PATH="${CMAKE_BINARY_DIR}/fonts/")
target_include_directories(2048 PRIVATE ${CMAKE_SOURCE_DIR}/shaders)
target_include_directories(2048 PRIVATE ${CMAKE_SOURCE_DIR}/fonts)

target_link_libraries(
	2048
	PRIVATE
	freetype-gl
	GLEW::GLEW
	glfw
)

file(COPY ${CMAKE_SOURCE_DIR}/fonts DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})