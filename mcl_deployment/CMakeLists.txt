# Set component name.
SET(MCL_COMPONENT "deployment")

# Set path of MCL Deployment Component's root Cmake directory.
SET(MCL_DEPLOYMENT_CMAKE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Set MCL_DEPLOYMENT_SOURCES to collect source files from different locations.
SET(MCL_DEPLOYMENT_SOURCES "" CACHE INTERNAL "MCL_DEPLOYMENT_SOURCES" FORCE)
SET(MCL_DEPLOYMENT_LIBS "" CACHE INTERNAL "MCL_DEPLOYMENT_LIBS" FORCE)
SET(MCL_DEPLOYMENT_INCLUDE_DIRECTORIES "" CACHE INTERNAL "MCL_DEPLOYMENT_INCLUDE_DIRECTORIES" FORCE)

IF(MCL_DEPLOYMENT)
    MESSAGE(STATUS "MCL Deployment build is ${MCL_DEPLOYMENT}.")
    ADD_DEFINITIONS(-DMCL_DEPLOYMENT_BUILD=1)
ENDIF()

IF(MCL_STATICLIB)
    SET(MCL_USER_DEFINED_DYNAMIC_OR_STATIC STATIC)
    ADD_DEFINITIONS(-DMCL_STATICLIB=1)
ELSE()
    SET(MCL_USER_DEFINED_DYNAMIC_OR_STATIC SHARED)
    ADD_DEFINITIONS(-DMCL_STATICLIB=0)
ENDIF()

MESSAGE(STATUS "MCL Deployment will be built as ${MCL_USER_DEFINED_DYNAMIC_OR_STATIC} library.")

# MCL DEPLOYMENT sources.
ADD_SUBDIRECTORY(src)

IF(MCL_TEST AND RUBY_FOUND)
    # Turn on CMake testing capabilities.
    ENABLE_TESTING()
    ADD_SUBDIRECTORY(test)
ENDIF()
       
# Doxygen.
IF(MCL_DOC)
    ADD_SUBDIRECTORY(doc)   
    ADD_DEPENDENCIES(mcl_deployment mcl_deployment_doc)    
    MESSAGE(STATUS "Creation of reference documentation is enabled.")
ELSE()
    MESSAGE(STATUS "Creation of reference documentation is disabled.")
ENDIF()

# Get all interface header files of mcl deployment component.     
FILE(GLOB MCL_DEPLOYMENT_INTERFACE_HEADER_FILES "${MCL_DEPLOYMENT_CMAKE_ROOT_DIR}/include/mcl_deployment/*.h")

# Install directory for interface header files of mcl deployment component.
INSTALL(FILES ${MCL_DEPLOYMENT_INTERFACE_HEADER_FILES} DESTINATION "${PACKAGE_DESTINATION_INCLUDE}/mcl_deployment")
