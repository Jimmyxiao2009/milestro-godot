cmake_minimum_required(VERSION 3.21)
project(milestro_smoke LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(MILESTRO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Milestro" CACHE PATH "")
set(MILESTRO_LIB_DIR "${MILESTRO_ROOT}/cmake-build-relwithdebinfo/lib" CACHE PATH "")

add_executable(milestro_smoke smoke_main.cpp)
target_include_directories(milestro_smoke PRIVATE
    "${MILESTRO_ROOT}/include"
    "${MILESTRO_ROOT}/include/Milestro/game"
    "${MILESTRO_ROOT}/include/Milestro/common"
)
target_compile_definitions(milestro_smoke PRIVATE MILESTRO_DLL)
find_library(MILESTRO_LIBRARY NAMES Milestro libMilestro PATHS "${MILESTRO_LIB_DIR}" NO_DEFAULT_PATH REQUIRED)
target_link_libraries(milestro_smoke PRIVATE "${MILESTRO_LIBRARY}")
set_target_properties(milestro_smoke PROPERTIES
    BUILD_RPATH "${MILESTRO_LIB_DIR}"
    INSTALL_RPATH "${MILESTRO_LIB_DIR}"
    BUILD_WITH_INSTALL_RPATH TRUE
)
