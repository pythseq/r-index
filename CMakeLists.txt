cmake_minimum_required(VERSION 2.6)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

#set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/.. )

project (s-rlbwt)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/internal)

# check for SDSL
find_library(SDSL_LIB sdsl)
if (NOT SDSL_LIB)
    message("sdsl-lite not found, building from submodule (hopefully you did git clone --recursive)")
    add_subdirectory(sdsl-lite)
    include_directories(${CMAKE_BINARY_DIR}/sdsl-lite/include/)
    include_directories(${CMAKE_BINARY_DIR}/sdsl-lite/external/libdivsufsort/include)
    LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/sdsl-lite/lib)
    LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/sdsl-lite/external/libdivsufsort/lib)
else()
    find_library(DSS divsufsort)
    find_library(DSS64 divsufsort64)
    message("sdsl: " ${SDSL_LIB})
    message("divsufsort: " ${DSS})
    message("divsufsort64: " ${DSS64})
endif()

message("Building in ${CMAKE_BUILD_TYPE} mode")

set(CMAKE_CXX_FLAGS "--std=c++17")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -g -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "-g -Ofast -fstrict-aliasing -march=native -DNDEBUG")
# set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -fstrict-aliasing -march=native -DNDEBUG -DLF_STATS")  # Uncomment this line in order to enable LF_STATS.
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -ggdb -Ofast -fstrict-aliasing -march=native")


add_executable(ri-build EXCLUDE_FROM_ALL ri-build.cpp)
TARGET_LINK_LIBRARIES(ri-build sdsl)
TARGET_LINK_LIBRARIES(ri-build divsufsort)
TARGET_LINK_LIBRARIES(ri-build divsufsort64)

add_executable(ri-locate EXCLUDE_FROM_ALL ri-locate.cpp)
TARGET_LINK_LIBRARIES(ri-locate sdsl)
TARGET_LINK_LIBRARIES(ri-locate divsufsort)
TARGET_LINK_LIBRARIES(ri-locate divsufsort64)

add_executable(ri-count EXCLUDE_FROM_ALL ri-count.cpp)
TARGET_LINK_LIBRARIES(ri-count sdsl)
TARGET_LINK_LIBRARIES(ri-count divsufsort)
TARGET_LINK_LIBRARIES(ri-count divsufsort64)

add_executable(ri-space EXCLUDE_FROM_ALL ri-space.cpp)
TARGET_LINK_LIBRARIES(ri-space sdsl)
TARGET_LINK_LIBRARIES(ri-space divsufsort)
TARGET_LINK_LIBRARIES(ri-space divsufsort64)

add_executable(ri-buildfasta ri-buildfasta.cpp)
TARGET_LINK_LIBRARIES(ri-buildfasta sdsl)
TARGET_LINK_LIBRARIES(ri-buildfasta divsufsort)
TARGET_LINK_LIBRARIES(ri-buildfasta divsufsort64)
TARGET_LINK_LIBRARIES(ri-buildfasta z)

add_executable(ri-align ri-align.cpp)
TARGET_LINK_LIBRARIES(ri-align sdsl)
TARGET_LINK_LIBRARIES(ri-align divsufsort)
TARGET_LINK_LIBRARIES(ri-align divsufsort64)
TARGET_LINK_LIBRARIES(ri-align z)
