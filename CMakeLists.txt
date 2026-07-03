cmake_minimum_required(VERSION 4.2)
project(Avalon)

set(CMAKE_CXX_STANDARD 20)

option(AV_BUILD_APPS "Build Avalon applications" ${PROJECT_IS_TOP_LEVEL})

include(ThirdParty/Dependencies.cmake)

if(AV_BUILD_APPS)
    add_subdirectory(Apps)
endif()

add_subdirectory(Avalon)