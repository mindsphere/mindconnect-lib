# Set component name.
SET(MCL_COMPONENT "data_lake")

# Set path of MCL Data Lake's root Cmake directory.
SET(MCL_DATA_LAKE_CMAKE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Set MCL_DATA_LAKE_SOURCES to collect source files from different locations.
SET(MCL_DATA_LAKE_SOURCES "" CACHE INTERNAL "MCL_DATA_LAKE_SOURCES" FORCE)
SET(MCL_DATA_LAKE_LIBS "" CACHE INTERNAL "MCL_DATA_LAKE_LIBS" FORCE)
SET(MCL_DATA_LAKE_INCLUDE_DIRECTORIES "" CACHE INTERNAL "MCL_DATA_LAKE_INCLUDE_DIRECTORIES" FORCE)

IF(MCL_DATA_LAKE)
    MESSAGE(STATUS "MCL Data Lake build is ${MCL_DATA_LAKE}.")
    ADD_DEFINITIONS(-DMCL_DATA_LAKE_BUILD=1)
ENDIF()

IF(MCL_STATICLIB)
    SET(MCL_USER_DEFINED_DYNAMIC_OR_STATIC STATIC)
    ADD_DEFINITIONS(-DMCL_STATICLIB=1)
ELSE()
    SET(MCL_USER_DEFINED_DYNAMIC_OR_STATIC SHARED)
    ADD_DEFINITIONS(-DMCL_STATICLIB=0)
ENDIF()

MESSAGE(STATUS "MCL Data Lake will be built as ${MCL_USER_DEFINED_DYNAMIC_OR_STATIC} library.")

# MCL Data Lake sources.
ADD_SUBDIRECTORY(src)

IF(MCL_TEST AND RUBY_FOUND)
    # Turn on CMake testing capabilities.
    ENABLE_TESTING()
    ADD_SUBDIRECTORY(test)
ENDIF()
       
# Doxygen.
IF(MCL_DOC)
    ADD_SUBDIRECTORY(doc)   
    ADD_DEPENDENCIES(mcl_data_lake mcl_data_lake_doc)	
    MESSAGE(STATUS "Creation of reference documentation is enabled.")
ELSE()
    MESSAGE(STATUS "Creation of reference documentation is disabled.")
ENDIF()

# Get all interface header files of mcl data lake component.     
FILE(GLOB MCL_DATA_LAKE_INTERFACE_HEADER_FILES "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/include/mcl_data_lake/*.h")

# Install directory for interface header files of mcl data lake component.
INSTALL(FILES ${MCL_DATA_LAKE_INTERFACE_HEADER_FILES} DESTINATION "${PACKAGE_DESTINATION_INCLUDE}/mcl_data_lake")
