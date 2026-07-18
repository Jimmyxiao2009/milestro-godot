cmake_minimum_required(VERSION 3.21)
project(milestro_godot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------
set(MILESTRO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../Milestro" CACHE PATH "Path to Milestro native tree")
set(MILESTRO_LIB_DIR "${MILESTRO_ROOT}/cmake-build-relwithdebinfo/lib" CACHE PATH "Directory containing libMilestro")
set(GODOTCPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/godot-cpp" CACHE PATH "godot-cpp source")
set(GODOTCPP_CUSTOM_API_FILE "${CMAKE_CURRENT_SOURCE_DIR}/extension_api.json" CACHE FILEPATH "Godot extension_api.json")
set(GODOTCPP_GIT_TAG "master" CACHE STRING "godot-cpp branch or tag used when GODOTCPP_DIR is missing")
option(GODOTCPP_FETCH "Fetch godot-cpp automatically when it is not present" ON)
set(GODOTCPP_PRECISION "single" CACHE STRING "" FORCE)
set(GODOTCPP_TARGET "template_debug" CACHE STRING "godot-cpp target: template_debug|template_release")

# ---------------------------------------------------------------------------
# Platform triple used in output names (Godot .gdextension convention)
# ---------------------------------------------------------------------------
if(WIN32)
    set(MILESTRO_GDEXT_PLATFORM "windows")
    set(MILESTRO_GDEXT_PREFIX "")
    set(MILESTRO_GDEXT_SUFFIX ".dll")
    set(MILESTRO_NATIVE_LIB_NAME "libMilestro.dll")
elseif(APPLE)
    set(MILESTRO_GDEXT_PLATFORM "macos")
    set(MILESTRO_GDEXT_PREFIX "lib")
    set(MILESTRO_GDEXT_SUFFIX ".dylib")
    set(MILESTRO_NATIVE_LIB_NAME "libMilestro.dylib")
elseif(UNIX)
    set(MILESTRO_GDEXT_PLATFORM "linux")
    set(MILESTRO_GDEXT_PREFIX "lib")
    set(MILESTRO_GDEXT_SUFFIX ".so")
    set(MILESTRO_NATIVE_LIB_NAME "libMilestro.so")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Arch
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
    set(MILESTRO_GDEXT_ARCH "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
    set(MILESTRO_GDEXT_ARCH "x86_64")
else()
    set(MILESTRO_GDEXT_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
endif()

# On Apple, honour CMAKE_OSX_ARCHITECTURES when set to a single arch
if(APPLE AND CMAKE_OSX_ARCHITECTURES)
    list(LENGTH CMAKE_OSX_ARCHITECTURES _arch_count)
    if(_arch_count EQUAL 1)
        list(GET CMAKE_OSX_ARCHITECTURES 0 _osx_arch)
        if(_osx_arch STREQUAL "arm64")
            set(MILESTRO_GDEXT_ARCH "arm64")
        elseif(_osx_arch STREQUAL "x86_64")
            set(MILESTRO_GDEXT_ARCH "x86_64")
        endif()
    endif()
endif()

set(MILESTRO_GDEXT_OUTPUT_NAME
    "milestro.${MILESTRO_GDEXT_PLATFORM}.${GODOTCPP_TARGET}.${MILESTRO_GDEXT_ARCH}"
)
set(MILESTRO_GDEXT_BIN_SUBDIR
    "${CMAKE_CURRENT_SOURCE_DIR}/addons/milestro/bin/${MILESTRO_GDEXT_PLATFORM}-${MILESTRO_GDEXT_ARCH}"
)

message(STATUS "GDExtension platform=${MILESTRO_GDEXT_PLATFORM} arch=${MILESTRO_GDEXT_ARCH}")
message(STATUS "GDExtension output=${MILESTRO_GDEXT_PREFIX}${MILESTRO_GDEXT_OUTPUT_NAME}${MILESTRO_GDEXT_SUFFIX}")
message(STATUS "Install dir=${MILESTRO_GDEXT_BIN_SUBDIR}")

# ---------------------------------------------------------------------------
# godot-cpp
# ---------------------------------------------------------------------------
if(NOT EXISTS "${GODOTCPP_DIR}/CMakeLists.txt")
    if(NOT GODOTCPP_FETCH)
        message(FATAL_ERROR "godot-cpp is missing at ${GODOTCPP_DIR}. Set GODOTCPP_DIR or enable GODOTCPP_FETCH.")
    endif()
    include(FetchContent)
    message(STATUS "godot-cpp not found; fetching ${GODOTCPP_GIT_TAG} from GitHub")
    FetchContent_Declare(godotcpp
        GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
        GIT_TAG ${GODOTCPP_GIT_TAG}
        GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(godotcpp)
    if(NOT godotcpp_POPULATED)
        FetchContent_Populate(godotcpp)
    endif()
    set(GODOTCPP_DIR "${godotcpp_SOURCE_DIR}")
endif()
if(EXISTS "${GODOTCPP_CUSTOM_API_FILE}")
    set(GODOTCPP_CUSTOM_API_FILE "${GODOTCPP_CUSTOM_API_FILE}" CACHE FILEPATH "" FORCE)
endif()
add_subdirectory(${GODOTCPP_DIR} godot-cpp-build EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------
# Extension
# ---------------------------------------------------------------------------
add_library(milestro_godot SHARED
    src/register_types.cpp
    src/milestro_binding.cpp
    src/milestro_font.cpp
    src/milestro_text_style.cpp
    src/milestro_paragraph_style.cpp
    src/milestro_strut_style.cpp
    src/milestro_paragraph_builder.cpp
    src/milestro_paragraph.cpp
    src/milestro_canvas.cpp
    src/milestro_image.cpp
    src/milestro_path.cpp
    src/milestro_vertex_data.cpp
    src/milestro_svg.cpp
    src/milestro_text_draw_snapshot.cpp
    src/milestro_reusable_text_draw_snapshot.cpp
    src/milestro_input_box.cpp
    src/milestro_input_box_draw_snapshot.cpp
    src/milestro_native_compat.cpp
    src/milestro_font_family_info.cpp
    src/milestro_font_face_info.cpp
    src/milestro_unicode_normalizer.cpp
    src/milestro_unicode_segmenter.cpp
    src/milestro_unicode_transliterator.cpp
    src/milestro_string_comparator.cpp
)

target_include_directories(milestro_godot PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${MILESTRO_ROOT}/include"
    "${MILESTRO_ROOT}/include/Milestro/game"
    "${MILESTRO_ROOT}/include/Milestro/common"
    "${MILESTRO_ROOT}/ext/skia"
    "${MILESTRO_ROOT}/ext/spdlog/include"
)

target_compile_definitions(milestro_godot PRIVATE MILESTRO_DLL)

# Find prebuilt native Milestro
if(WIN32)
    # Prefer lib dir, then bin (Windows install layout)
    find_library(MILESTRO_LIBRARY
        NAMES Milestro libMilestro
        PATHS
            "${MILESTRO_LIB_DIR}"
            "${MILESTRO_ROOT}/cmake-build-relwithdebinfo/lib"
            "${MILESTRO_ROOT}/cmake-build-relwithdebinfo/bin"
            "${MILESTRO_ROOT}/build_native/lib"
            "${MILESTRO_ROOT}/build_native/bin"
        NO_DEFAULT_PATH
    )
    # Also search for the import library next to the DLL
    if(NOT MILESTRO_LIBRARY)
        file(GLOB_RECURSE _milestro_candidates
            "${MILESTRO_ROOT}/**/Milestro.lib"
            "${MILESTRO_ROOT}/**/libMilestro.lib"
            "${MILESTRO_ROOT}/**/libMilestro.dll.a"
        )
        if(_milestro_candidates)
            list(GET _milestro_candidates 0 MILESTRO_LIBRARY)
        endif()
    endif()
else()
    find_library(MILESTRO_LIBRARY
        NAMES Milestro libMilestro
        PATHS
            "${MILESTRO_LIB_DIR}"
            "${MILESTRO_ROOT}/cmake-build-relwithdebinfo/lib"
            "${MILESTRO_ROOT}/build_native/lib"
            "${MILESTRO_ROOT}/cmake-build-relwithdebinfo"
        NO_DEFAULT_PATH
    )
endif()

if(NOT MILESTRO_LIBRARY)
    message(FATAL_ERROR "libMilestro not found. Set MILESTRO_LIB_DIR to the directory containing the native library.")
endif()
message(STATUS "Linking native Milestro: ${MILESTRO_LIBRARY}")

target_link_libraries(milestro_godot PRIVATE godot-cpp "${MILESTRO_LIBRARY}")

set_target_properties(milestro_godot PROPERTIES
    OUTPUT_NAME "${MILESTRO_GDEXT_OUTPUT_NAME}"
    PREFIX "${MILESTRO_GDEXT_PREFIX}"
    SUFFIX "${MILESTRO_GDEXT_SUFFIX}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(APPLE)
    set_target_properties(milestro_godot PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
    target_link_options(milestro_godot PRIVATE "LINKER:-rpath,@loader_path")
elseif(UNIX)
    set_target_properties(milestro_godot PROPERTIES
        BUILD_RPATH "\$ORIGIN"
        INSTALL_RPATH "\$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

# Locate the actual shared library file to copy next to the extension
get_filename_component(MILESTRO_LIBRARY_DIR "${MILESTRO_LIBRARY}" DIRECTORY)
if(WIN32)
    # Import lib may be .lib; real DLL often sits in same or sibling bin/
    file(GLOB _dll_candidates
        "${MILESTRO_LIBRARY_DIR}/libMilestro.dll"
        "${MILESTRO_LIBRARY_DIR}/Milestro.dll"
        "${MILESTRO_LIBRARY_DIR}/../bin/libMilestro.dll"
        "${MILESTRO_LIBRARY_DIR}/../bin/Milestro.dll"
    )
    if(_dll_candidates)
        list(GET _dll_candidates 0 MILESTRO_SHARED_FILE)
    else()
        set(MILESTRO_SHARED_FILE "${MILESTRO_LIBRARY}")
    endif()
else()
    set(MILESTRO_SHARED_FILE "${MILESTRO_LIBRARY}")
endif()

# ICU data (optional but recommended)
set(MILESTRO_ICU_DAT "")
foreach(_icu_candidate
    "${MILESTRO_ROOT}/ext/icu-cmake/common/icudtl.dat"
    "${MILESTRO_LIBRARY_DIR}/icudtl.dat"
    "${MILESTRO_ROOT}/cmake-build-relwithdebinfo/lib/icudtl.dat"
)
    if(EXISTS "${_icu_candidate}")
        set(MILESTRO_ICU_DAT "${_icu_candidate}")
        break()
    endif()
endforeach()

set(_install_cmds
    COMMAND ${CMAKE_COMMAND} -E make_directory "${MILESTRO_GDEXT_BIN_SUBDIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:milestro_godot>" "${MILESTRO_GDEXT_BIN_SUBDIR}/"
    COMMAND ${CMAKE_COMMAND} -E copy "${MILESTRO_SHARED_FILE}" "${MILESTRO_GDEXT_BIN_SUBDIR}/${MILESTRO_NATIVE_LIB_NAME}"
)
if(MILESTRO_ICU_DAT)
    list(APPEND _install_cmds
        COMMAND ${CMAKE_COMMAND} -E copy "${MILESTRO_ICU_DAT}" "${MILESTRO_GDEXT_BIN_SUBDIR}/icudtl.dat"
    )
endif()
add_custom_command(TARGET milestro_godot POST_BUILD
    ${_install_cmds}
    COMMENT "Installing GDExtension + libMilestro into ${MILESTRO_GDEXT_BIN_SUBDIR}"
)
