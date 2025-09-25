cmake_minimum_required(VERSION 3.15)
project(PyspielExtensionsSharedLibrary VERSION 1.0.0)

# --- We link with open_spiel, which uses C++17. ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Generate compile_commands.json file for clangd. ---
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# --- Set Compiler Flags Based on Build Type ---
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Build type is Release. Applying performance optimizations.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -march=native -fomit-frame-pointer -funroll-loops -finline-functions -fvectorize -fslp-vectorize")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -O3 -DNDEBUG -march=native -fomit-frame-pointer")

elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Build type is Debug. Applying debug flags.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -g -O0")

elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(STATUS "Build type is RelWithDebInfo. Applying optimized flags with debug symbols.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -DNDEBUG -march=native -funroll-loops -finline-functions -fvectorize -fslp-vectorize")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -O2 -g -DNDEBUG -march=native")

else()
    message(STATUS "No build type specified. Defaulting to Release optimizations.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -march=native -fomit-frame-pointer")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -O3 -DNDEBUG -march=native -fomit-frame-pointer")
endif()
# --- End Compiler Flags ---

# --- Define Source Files and Directories ---

# Define variables for key paths relative to this CMakeLists.txt file.
set(EXTENSIONS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../extensions)
set(OPEN_SPIEL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/open_spiel)

# --- External packages ---
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# --- Stage 1: Compile Core C++ to Object Files ---
add_library(pyspiel_extensions_core OBJECT
    ${EXTENSIONS_DIR}/backward_induction/backward_induction.cc
    ${EXTENSIONS_DIR}/subtraction_game/subtraction_game.cc
)

# Add include directories needed to compile the core logic.
target_include_directories(pyspiel_extensions_core PRIVATE
    ${EXTENSIONS_DIR}/backward_induction
    ${EXTENSIONS_DIR}/subtraction_game
    ${OPEN_SPIEL_DIR}
    ${OPEN_SPIEL_DIR}/open_spiel/abseil-cpp/
    ${OPEN_SPIEL_DIR}/open_spiel/json/include
)

# Object files destined for a shared library must be compiled with -fPIC.
set_target_properties(pyspiel_extensions_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

# --- Stage 2: Create the Python Module ---
# Create the final Python MODULE library. This is the actual .so file.
add_library(_core MODULE
    ${EXTENSIONS_DIR}/bindings.cc
    ${EXTENSIONS_DIR}/backward_induction/backward_induction_pybind11.cc
    ${EXTENSIONS_DIR}/subtraction_game/subtraction_game_pybind11.cc
)

# Add include directories needed for the pybind11 wrapper code.
target_include_directories(_core PRIVATE
    ${OPEN_SPIEL_DIR}/pybind11/include # pybind11 headers
    ${Python3_INCLUDE_DIRS}           # Python.h
    ${EXTENSIONS_DIR}/
    ${EXTENSIONS_DIR}/backward_induction   # Local extension headers
    ${EXTENSIONS_DIR}/subtraction_game
    ${OPEN_SPIEL_DIR}                 # Core OpenSpiel headers
    ${OPEN_SPIEL_DIR}/open_spiel/abseil-cpp/
    ${OPEN_SPIEL_DIR}/open_spiel/json/include
)

# --- Configure Python Module Filename ---
# Ensure the output file is named correctly for Python import (e.g., "shared_library.so").
set_target_properties(_core PROPERTIES PREFIX "")
if (WIN32)
    set_target_properties(_core PROPERTIES SUFFIX ".pyd")
endif()
set_target_properties(_core PROPERTIES
    INSTALL_RPATH "$ORIGIN"
)

# --- Link All Dependencies ---
# Define the pre-built libopen_spiel.so as an IMPORTED target.
add_library(open_spiel_imported SHARED IMPORTED)
set_target_properties(open_spiel_imported PROPERTIES
    IMPORTED_LOCATION "${OPEN_SPIEL_DIR}/build/libopen_spiel.so"
)

# Link the final module.
target_link_libraries(_core PRIVATE
    # 1. Link the object files from our core C++ logic using a generator expression.
    $<TARGET_OBJECTS:pyspiel_extensions_core>

    # 2. Link the pre-built OpenSpiel shared library.
    open_spiel_imported

    # 3. Link the modern Python::Python target provided by find_package(Python3).
    Python::Python
)

# --- INSTALLATION RULES ---

install(TARGETS _core LIBRARY DESTINATION pyspiel_extensions/shared_library)
install(FILES ${OPEN_SPIEL_DIR}/build/libopen_spiel.so DESTINATION pyspiel_extensions/shared_library)
