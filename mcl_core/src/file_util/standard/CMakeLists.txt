
#MCL needs just these three to be set in PARENT_SCOPE.
SET(MCL_FILE_UTIL_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
SET(MCL_FILE_UTIL_LIBRARIES "" PARENT_SCOPE)
SET(MCL_FILE_UTIL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/file_util.c" PARENT_SCOPE)
