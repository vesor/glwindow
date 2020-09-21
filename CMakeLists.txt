
cmake_minimum_required(VERSION 2.8)
project(glwindow)

add_subdirectory(thirdparty)

set (CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

#find_package(CUDA)
find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
#find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SRC_CODE
    "src/*.cpp"
    "src/*.cxx"
    "src/*.cc"
    "src/*.c"
    "deps/*.c"
)

include_directories(
  include
  src
  deps
  ${FREETYPE_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIR}
  #${GLFW3_INCLUDE_DIR}  # Maybe needed for glfw3 before 3.2
)

set(SUPPORT_LIBS glfw GL freetype pthread)
if ((${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    OR (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64"))
    list(APPEND SUPPORT_LIBS EGL)
    link_directories(/opt/miivii/lib)
endif()


add_executable(${PROJECT_NAME} ${SRC_CODE})

target_link_libraries(${PROJECT_NAME} PRIVATE
  #${OpenCV_LIBS}
  #${CUDA_LIBRARIES}
  ${SUPPORT_LIBS} 
)