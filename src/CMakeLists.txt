### ---[ global CMake
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(nxui)
string(TOLOWER ${PROJECT_NAME} LOWER_PROJECT_NAME)

include(${CMAKE_SOURCE_DIR}/cmake/prj_build_config.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/pcl_utils.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/pcl_targets.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/prj_utils.cmake)

# In case the user does not setup CMAKE_BUILD_TYPE, assume it's RelWithDebInfo
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "build type default to Release" FORCE)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")

SET_INSTALL_DIRS()

#set(PCL_BINARY_DIR ${CMAKE_BINARY_DIR})
#set(DUIX_INSTALL_LIB_DIR "lib")
#set(DUIX_INSTALL_BIN_DIR "bin")
set(DUIX_OUTPUT_LIB_DIR ${CMAKE_BINARY_DIR}/${INSTALLATION_LIB_DIR})
set(DUIX_OUTPUT_BIN_DIR ${CMAKE_BINARY_DIR}/${INSTALLATION_BIN_DIR})
make_directory(${DUIX_OUTPUT_LIB_DIR})
make_directory(${DUIX_OUTPUT_BIN_DIR})
set(INSTALLATION_INCLUDE_DIR "include")
set(DUIX_OUTPUT_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${INSTALLATION_INCLUDE_DIR})
make_directory(${DUIX_OUTPUT_BIN_DIR})

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "possible configurations" FORCE)

if(WIN32)
  foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} CONFIG)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${DUIX_OUTPUT_LIB_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${DUIX_OUTPUT_BIN_DIR}")
    # ---[ Windows requires DLLs (shared libraries) to be installed in the same directory as executables
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${DUIX_OUTPUT_BIN_DIR}")
  endforeach(config)
else(WIN32)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${DUIX_OUTPUT_LIB_DIR}")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${DUIX_OUTPUT_BIN_DIR}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${DUIX_OUTPUT_LIB_DIR}")
endif(WIN32)

### ---[ Create the config.h file
include_directories(${DUIX_OUTPUT_INCLUDE_DIR})
link_directories(${DUIX_OUTPUT_LIB_DIR})


set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()


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
