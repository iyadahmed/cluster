cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(cluster)

find_package(glm REQUIRED)
if (MINGW)
    set(GLM_TARGET glm)
else()
    set(GLM_TARGET glm::glm)
endif ()

add_executable(cluster main.cpp hash.hpp indexed_mesh.hpp)
target_link_libraries(cluster ${GLM_TARGET})
