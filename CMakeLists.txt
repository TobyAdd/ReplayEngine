cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT ReplayEngine)
project(${PROJECT})

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_library(${PROJECT} SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT} PRIVATE
  libs/minhook/include
  libs/gd.h/include
  libs/gd.h/
  libs/imgui-hook
  libs/imgui-hook/imgui
  libs
)

add_subdirectory(libs/minhook)
add_subdirectory(libs/imgui-hook)
add_subdirectory(libs/cocos-headers)
target_link_libraries(${PROJECT} minhook imgui-hook opengl32 cocos2d)

target_link_libraries(${PROJECT} ${CMAKE_SOURCE_DIR}/libs/cocos-headers/cocos2dx/libcocos2d.lib)
target_link_libraries(${PROJECT} ${CMAKE_SOURCE_DIR}/libs/cocos-headers/extensions/libExtensions.lib)
target_link_libraries(${PROJECT} ${CMAKE_SOURCE_DIR}/libs/glew32.lib)

# add_custom_command(TARGET ${PROJECT} POST_BUILD
#                    COMMAND ${CMAKE_COMMAND} -E copy
#                        $<TARGET_FILE:${PROJECT}>
#                         E:/Games/GeometryDash/.library/ReplayEngine.dll)