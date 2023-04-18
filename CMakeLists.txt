# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)


include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(can_emulator)
target_compile_options(${PROJECT_NAME}.elf INTERFACE -Wno-error=format= -Wno-format)
target_compile_features(${PROJECT_NAME}.elf PRIVATE cxx_std_20)
