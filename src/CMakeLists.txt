### ---[ global CMake
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(bxui)
string(TOLOWER ${PROJECT_NAME} LOWER_PROJECT_NAME)

include(${CMAKE_SOURCE_DIR}/cmake/prj_build_config.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/pcl_utils.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/pcl_targets.cmake)
#include(${CMAKE_SOURCE_DIR}/cmake/duix_utils.cmake)

# In case the user does not setup CMAKE_BUILD_TYPE, assume it's RelWithDebInfo
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "build type default to Release" FORCE)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../${LOWER_PROJECT_NAME}-installation")
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../${LOWER_PROJECT_NAME}-installation")
ENDIF()


file (GLOB modules_subdirs */CMakeLists.txt)

foreach(subdir ${modules_subdirs})
    # get_filename_component (dir ${subdir} PATH)
    get_filename_component (absolute ${subdir} ABSOLUTE)
    get_filename_component (name ${subdir} NAME)
    STRING(REGEX REPLACE ${name} "" dir ${absolute})
    list(APPEND PRJ_MODULES_DIRS ${dir})
endforeach(subdir)
# Display the found subdirectories
message (STATUS "PRJ_MODULES_DIRS ${PRJ_MODULES_DIRS}")
# Add the subdirectories
foreach(subdir ${PRJ_MODULES_DIRS})
add_subdirectory(${subdir})
endforeach(subdir)

MESSAGE(STATUS ${CMAKE_INSTALL_PREFIX})
