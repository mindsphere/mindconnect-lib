# Set component name.
SET(MCL_COMPONENT "core")

# Set path of MCL's root Cmake directory.
SET(MCL_CORE_CMAKE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
 
# Set MCL_CORE_SOURCES to collect source files from different locations.
SET(MCL_CORE_SOURCES "" CACHE INTERNAL "MCL_CORE_SOURCES" FORCE)
SET(MCL_CORE_LIBS "" CACHE INTERNAL "MCL_CORE_LIBS" FORCE)
SET(MCL_CORE_INCLUDE_DIRECTORIES "" CACHE INTERNAL "MCL_CORE_INCLUDE_DIRECTORIES" FORCE)

ADD_DEFINITIONS(-DMCL_CORE_BUILD=1)

IF(MCL_STATICLIB)
    SET(MCL_CORE_DYNAMIC_OR_STATIC STATIC)
    ADD_DEFINITIONS(-DMCL_STATICLIB=1)
ELSE()
    SET(MCL_CORE_DYNAMIC_OR_STATIC SHARED)
    ADD_DEFINITIONS(-DMCL_STATICLIB=0)
ENDIF()

MESSAGE(STATUS "MCL Core will be built as ${MCL_CORE_DYNAMIC_OR_STATIC} library.")

ADD_SUBDIRECTORY(lib/cJSON)

OPTION(MCL_MOCKSERVER_INTEGRATION "MCL integration test with mock server" OFF)

IF(MCL_MOCKSERVER_INTEGRATION)
    ADD_DEFINITIONS(-DMCL_MOCKSERVER_INTEGRATION)
ENDIF()

# MCL Core sources.
ADD_SUBDIRECTORY(src)

IF(MCL_TEST OR MCL_MOCKSERVER_INTEGRATION)
    IF(RUBY_FOUND)
        # Turn on CMake testing capabilities.
        ENABLE_TESTING()
        MESSAGE(STATUS "Ruby found on path (${RUBY_FOUND}) and MCL testing is ${MCL_TEST}.")
    ELSE()    
        MESSAGE(STATUS "Ruby not found on path. Skipping MCL testing...")
    ENDIF()
    
    IF(MCL_TEST)
        ADD_SUBDIRECTORY(test)
    ELSE()
        ADD_SUBDIRECTORY(test/lib/CMock)
        ADD_SUBDIRECTORY(test/integration)
    ENDIF()
ENDIF()

IF(MCL_DOC)

    # Install README document.
    INSTALL(FILES "${MCL_CMAKE_ROOT_DIR}/README.md" DESTINATION ${PACKAGE_DESTINATION_DOC})
    
    ADD_SUBDIRECTORY(doc)
    ADD_DEPENDENCIES(mcl_core mcl_core_doc)    
    MESSAGE(STATUS "Creation of reference documentation is enabled.")
ELSE()
    MESSAGE(STATUS "Creation of reference documentation is disabled.")
ENDIF()

# Get all interface header files of mcl core component.     
FILE(GLOB MCL_CORE_INTERFACE_HEADER_FILES "${MCL_CORE_CMAKE_ROOT_DIR}/include/mcl_core/*.h")

# Install directory for interface header files of mcl core component.
INSTALL(FILES ${MCL_CORE_INTERFACE_HEADER_FILES} DESTINATION "${PACKAGE_DESTINATION_INCLUDE}/mcl_core")
    
MARK_AS_ADVANCED(MCL_CORE_INCLUDE_DIRECTORIES MCL_CORE_LIBS)
