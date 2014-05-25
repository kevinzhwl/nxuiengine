if(BUILD_examples)

    set(EXAMPLE_TARGET_NAME "sonicuidemo")

    include_directories("${CMAKE_BINARY_DIR}/include")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
    link_directories(${CMAKE_BINARY_DIR}/addon/lib)

    file(GLOB_RECURSE APP_SRCS "*.cpp")
    file(GLOB_RECURSE APP_INCS "*.h")
    file(GLOB APP_RES "res/*.*" "basicdemo.rc")

    set(ALL_SOURCES
        ${APP_SRCS} ${APP_INCS} ${APP_SKINS} ${APP_RES} ${RC_FILE})


    set(ENGINE_TARGET_NAME "nxuiengine")
    set(DUIX_TARGET_PLATFORM "Win32")
    set(DUIX_TRAGET_STATIC_NAME "${ENGINE_TARGET_NAME}-${DUIX_TARGET_PLATFORM}-${CMAKE_BUILD_TYPE}-static")

    set(ALL_LIBRARIES "${DUIX_TRAGET_STATIC_NAME}.lib")

    add_definitions("-DUNICODE -D_UNICODE")
    add_executable(${EXAMPLE_TARGET_NAME} WIN32 ${ALL_SOURCES})
    target_link_libraries(${EXAMPLE_TARGET_NAME} ${ALL_LIBRARIES})

    add_dependencies(${EXAMPLE_TARGET_NAME} ${DUIX_TRAGET_STATIC_NAME})

endif(BUILD_examples)

