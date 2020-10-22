# Add a target to generate API documentation with Doxygen.
FIND_PACKAGE(Doxygen)
MESSAGE(STATUS "Create and install the HTML based API documentation (requires Doxygen). Doxygen found = " ${DOXYGEN_FOUND})

SET(DOXYGEN_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/build/doc/mcl_core")
SET(DOXYGEN_DOCUMENTATION_DIRECTORY "${CMAKE_BINARY_DIR}/build/doc/mcl_core/html")

IF(DOXYGEN_FOUND)
    MESSAGE(STATUS "Using Doxygen (${DOXYGEN_EXECUTABLE}) to build reference documentation.")
    SET(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/mcl_core.doxyfile)
    SET(DOXYFILE ${DOXYGEN_WORKING_DIRECTORY}/mcl_core.doxyfile)
    CONFIGURE_FILE(${DOXYFILE_IN} ${DOXYFILE} @ONLY)

    ADD_CUSTOM_TARGET(mcl_core_doc
                      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/build/doc
                      COMMENT "Generating API documentation with Doxygen."
                      VERBATIM)

    # Install doxygen documents.
    INSTALL(DIRECTORY  ${DOXYGEN_DOCUMENTATION_DIRECTORY} DESTINATION "${PACKAGE_DESTINATION_DOC}/mcl_core")

ELSE()
    MESSAGE(FATAL_ERROR "Doxygen is required to build the documentation.")
ENDIF()
