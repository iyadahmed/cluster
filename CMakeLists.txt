cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(cluster)

add_subdirectory(extern/tiny_stl)

set(FORCE_BUNDLED_GLM NO CACHE BOOL "Force the use of glm library from the extern folder instead of system's")
if ((NOT ${FORCE_BUNDLED_GLM}) AND (NOT MSVC))
    find_package(glm)
endif ()

if (glm_FOUND)
    message(STATUS "Using system's GLM")
    if (MINGW)
        set(GLM_TARGET glm)
    else ()
        set(GLM_TARGET glm::glm)
    endif ()
else ()
    message(STATUS "Using bundled GLM")
    add_subdirectory(extern/glm)
    set(GLM_TARGET glm)
endif ()

add_executable(cluster main.cpp indexed_mesh.cpp hash.hpp indexed_mesh.hpp)
target_link_libraries(cluster ${GLM_TARGET} tiny_stl)
