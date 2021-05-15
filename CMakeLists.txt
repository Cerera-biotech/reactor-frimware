cmake_minimum_required(VERSION 3.16.0)   
# include(FetchContent)
# FetchContent_Declare(
#   espidflib
#   GIT_REPOSITORY https://github.com/UncleRus/esp-idf-lib.git
#   GIT_TAG        0.8.0
# )
# FetchContent_MakeAvailable(espidflib)
# set(EXTRA_COMPONENT_DIRS ${espidflib_SOURCE_DIR}/components)
# set(EXTRA_COMPONENT_DIRS ../esp-idf-lib/components)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(reactor-firmware)