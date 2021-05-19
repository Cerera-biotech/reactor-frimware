cmake_minimum_required(VERSION 3.16.0)   
include(FetchContent)
FetchContent_Declare(
  adssensor
  GIT_REPOSITORY https://github.com/longtengj/esp-idf-sensor
#   GIT_TAG        0.8.0
)
FetchContent_MakeAvailable(adssensor)
set(EXTRA_COMPONENT_DIRS ${adssensor_SOURCE_DIR})
# set(EXTRA_COMPONENT_DIRS ../esp-idf-lib/components)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(reactor-firmware)