cmake_minimum_required(VERSION 3.21)
project(milestro_godot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Paths
set(MILESTRO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../Milestro" CACHE PATH "Path to Milestro native tree")
set(MILESTRO_LIB_DIR "${MILESTRO_ROOT}/cmake-build-relwithdebinfo/lib" CACHE PATH "Directory containing libMilestro")
set(GODOTCPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/godot-cpp" CACHE PATH "godot-cpp source")

# Use the dumped Godot 4.7 extension API
set(GODOTCPP_CUSTOM_API_FILE "${CMAKE_CURRENT_SOURCE_DIR}/extension_api.json" CACHE FILEPATH "" FORCE)
set(GODOTCPP_PRECISION "single" CACHE STRING "" FORCE)
# Target template_debug for editor/headless runs
set(GODOTCPP_TARGET "template_debug" CACHE STRING "" FORCE)

add_subdirectory(${GODOTCPP_DIR} godot-cpp-build EXCLUDE_FROM_ALL)

add_library(milestro_godot SHARED
    src/register_types.cpp
    src/milestro_binding.cpp
)

target_include_directories(milestro_godot PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${MILESTRO_ROOT}/include"
    "${MILESTRO_ROOT}/include/Milestro/game"
    "${MILESTRO_ROOT}/include/Milestro/common"
)

target_compile_definitions(milestro_godot PRIVATE
    MILESTRO_DLL
)

# Link against prebuilt native Milestro + godot-cpp
find_library(MILESTRO_LIBRARY
    NAMES Milestro libMilestro
    PATHS "${MILESTRO_LIB_DIR}"
    NO_DEFAULT_PATH
    REQUIRED
)

target_link_libraries(milestro_godot PRIVATE
    godot-cpp
    "${MILESTRO_LIBRARY}"
)

# macOS dylib naming expected by .gdextension
set_target_properties(milestro_godot PROPERTIES
    OUTPUT_NAME "milestro.macos.template_debug"
    PREFIX "lib"
    SUFFIX ".dylib"
    BUILD_RPATH "@loader_path;@loader_path/.."
    INSTALL_RPATH "@loader_path;@loader_path/.."
    BUILD_WITH_INSTALL_RPATH TRUE
)

# Also allow loading libMilestro.dylib from the same bin folder
if(APPLE)
    target_link_options(milestro_godot PRIVATE
        "LINKER:-rpath,@loader_path"
    )
endif()

# Copy both libraries into addons/milestro/bin after build
set(MILESTRO_GDEXT_BIN "${CMAKE_CURRENT_SOURCE_DIR}/addons/milestro/bin")
add_custom_command(TARGET milestro_godot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${MILESTRO_GDEXT_BIN}"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:milestro_godot>" "${MILESTRO_GDEXT_BIN}/"
    COMMAND ${CMAKE_COMMAND} -E copy "${MILESTRO_LIBRARY}" "${MILESTRO_GDEXT_BIN}/libMilestro.dylib"
    COMMENT "Installing GDExtension binaries into addons/milestro/bin"
)
