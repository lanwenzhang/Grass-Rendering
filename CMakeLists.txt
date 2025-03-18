cmake_minimum_required(VERSION 3.12)

project(grassRendering)

set(CMAKE_CXX_STANDARD 17)

add_definitions (-DDEBUG)

file(GLOB ASSETS "./assets" "./thirdParty/bin/assimp/assimp-vc143-mtd.dll")
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})


include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/include)

link_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/lib)

add_subdirectory(wrapper)
add_subdirectory(application)
add_subdirectory(glframework)
add_subdirectory(imgui)

add_executable(grassRendering "main.cpp" "glad.c")

target_link_libraries(grassRendering glfw3.lib assimp-vc143-mtd.lib zlibstaticd.lib wrapper app fw imguilib)