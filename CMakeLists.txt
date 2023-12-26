# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(can_emulator)
# e.g. for espressif/button component:
# idf_component_get_property(${lib} espressif__button COMPONENT_LIB)
target_compile_options(${lib} PRIVATE -Wno-error=format)
target_compile_options(${PROJECT_NAME}.elf PRIVATE -Wno-error=format= -Wno-format)
# target_compile_features(${PROJECT_NAME}.elf PRIVATE cxx_std_20)
