set(SUBSYS_NAME examples)
set(SUBSYS_DESC "examples for nxuiengine")
#set(SUBSYS_DEPS )

set(DEFAULT TRUE)
set(REASON "Code examples are disabled by default.")
PCL_SUBSYS_OPTION(build ${SUBSYS_NAME} ${SUBSYS_DESC} ${DEFAULT} ${REASON})
PCL_SUBSYS_DEPEND(build ${SUBSYS_NAME} DEPS ${SUBSYS_DEPS})

if (build)

    # This looks for all examples/XXX/CMakeLists.txt
    file (GLOB examples_sbudirs */CMakeLists.txt)
    # Extract only relevant XXX and append it to PRJ_EXAMPLES_SUBDIRS
    foreach(subdir ${examples_sbudirs})
        get_filename_component (absolute ${subdir} ABSOLUTE)
        get_filename_component (name ${subdir} NAME)
        STRING(REGEX REPLACE ${name} "" dir ${absolute})
        list(APPEND PRJ_EXAMPLES_SUBDIRS ${dir})
    endforeach(subdir)
    # Display the found subdirectories
    message (STATUS "PRJ_EXAMPLES_SUBDIRS ${PRJ_EXAMPLES_SUBDIRS}")
    # Add the subdirectories
    foreach(subdir ${PRJ_EXAMPLES_SUBDIRS})
    add_subdirectory(${subdir})
    endforeach(subdir)

endif (build)
