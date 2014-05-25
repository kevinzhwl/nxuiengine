
macro(add_prj_definitions)

add_definitions(-DWIN32)
add_definitions(-D_WINDOWS)
add_definitions(-D_USRDLL)

add_definitions("-DUNICODE -D_UNICODE")

add_definitions("/W3 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS /bigobj")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
add_definitions(-D_DEBUG)
add_definitions(-DDEBUG)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
add_definitions(-DNDEBUG)
endif()


endmacro(add_prj_definitions)
