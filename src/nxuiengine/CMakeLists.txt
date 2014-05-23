
set(SUBSYS_NAME bxuiengine)
set(SUBSYS_DESC "bxuiengine")
#set(SUBSYS_DEPS )

set(DEFAULT TRUE)
set(REASON "build engine are enabled by default.")
PCL_SUBSYS_OPTION(build ${SUBSYS_NAME} ${SUBSYS_DESC} ${DEFAULT} ${REASON})
PCL_SUBSYS_DEPEND(build ${SUBSYS_NAME} DEPS ${SUBSYS_DEPS})

if(build)

    set(ENGINE_TARGET_NAME "${SUBSYS_NAME}")

    set(DUIX_TARGET_PLATFORM "Win32")

    add_definitions("-DUNICODE -D_UNICODE")

    if(WIN32)
        # lots of warnings with cl.exe right now, use /W1
        add_definitions("/W3 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS /bigobj")
    endif(WIN32)
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

    #set the C/C++ include path to the "include" directory
    #include_directories("${PROJECT_SOURCE_DIR}/include")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/addon/lib)

    file( GLOB_RECURSE HEADER_INCS "*.h" "*.hpp" "*.tlh" "*.tli")

    file( GLOB_RECURSE ENGINE_INCS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

    file( GLOB_RECURSE ENGINE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )

    file( GLOB PUGIXML_INCS "${CMAKE_CURRENT_SOURCE_DIR}/addon/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/addon/include/*.hpp")
    file( GLOB PUGIXML_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/addon/src/*.cpp" )
    file( GLOB PUGIXML_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/addon/lib/*.lib" )

    set(CPP_SRCS
    ${ENGINE_SRCS}
    ${ENGINE_EXTRA_SRCS}
    ${PUGIXML_SRCS}
    )

    set(ALL_INCLUDES
        ${ENGINE_INCS}
        ${ENGINE_EXTRA_INCS}
        ${PUGIXML_INCS}
    )

    set(ALL_SOURCES
        ${MFC_APP_SRCS}
        ${CPP_SRCS}
        ${HEADER_INCS}
    )

    set(ALL_LIBRARIES
       ${PUGIXML_LIBS}
        )


    if(BUILD_duixengine_static)
        set(DUIX_TRAGET_STATIC_NAME "${ENGINE_TARGET_NAME}-${DUIX_TARGET_PLATFORM}-${CMAKE_BUILD_TYPE}-static")
        add_library("${DUIX_TRAGET_STATIC_NAME}" STATIC ${ALL_SOURCES} )
        #target_link_libraries("${DUIX_TRAGET_STATIC_NAME}" ${ALL_LIBRARIES})
    endif(BUILD_duixengine_static)


    if(BUILD_duixengine_shared)
        set(DUIX_TRAGET_SHARED_NAME "${ENGINE_TARGET_NAME}-${DUIX_TARGET_PLATFORM}-${CMAKE_BUILD_TYPE}-shared")
        #ADD_MSVC_PRECOMPILED_HEADER("include/dui/duistd.h" "src/duistd.cpp" ALL_SOURCES)
        add_library(${DUIX_TRAGET_SHARED_NAME} SHARED ${ALL_SOURCES} )
        target_link_libraries(${DUIX_TRAGET_SHARED_NAME} ${ALL_LIBRARIES})
        set_property(TARGET ${DUIX_TRAGET_SHARED_NAME}
         PROPERTY COMPILE_DEFINITIONS WIN32 NDEBUG _WINDOWS _USRDLL SONICUI_EXPORTS DLL_DUI)
#        PCL_ADD_LIBRARY(${LIB_NAME} ${SUBSYS_NAME} ${ALL_SOURCES} ${ALL_INCLUDES} )
#        target_link_libraries(${LIB_NAME} ${ALL_LIBRARIES} )
    #    PCL_MAKE_PKGCONFIG(${LIB_NAME} ${SUBSYS_NAME} "${SUBSYS_DESC}" "${SUBSYS_DEPS}" "" "" "" "")
        # Install include files
#        PCL_ADD_INCLUDES(${SUBSYS_NAME} ${SUBSYS_NAME} ${ALL_INCLUDES})

    endif(BUILD_duixengine_shared)



#/////////////////////////////////////////////////////////////////
# installation setting


endif(build)