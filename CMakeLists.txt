# The sole  purpose of this file is to create compile_commands.json file for IDEs

cmake_minimum_required(VERSION 3.15)

project(pyspiel_extensions_dev)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(pyspiel-extensions-shared-library)
