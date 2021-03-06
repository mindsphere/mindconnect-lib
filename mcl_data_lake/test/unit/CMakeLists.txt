#Set name of the test framework.
SET(TEST_FRAMEWORK unity_and_cmock)

#Set paths.
SET(DATA_LAKE_MOCK_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/mock")
SET(TEST_CASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
SET(TEST_RUNNER_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/testrunner")
SET(GENERATE_TEST_RUNNER_SCRIPT_PATH "mcl_core/test/lib/CMock/vendor/unity/auto/generate_test_runner.rb")
SET(TEST_TYPE "unit")

#Create mock and test runner directories.
FILE(MAKE_DIRECTORY ${DATA_LAKE_MOCK_DIRECTORY})
FILE(MAKE_DIRECTORY ${TEST_RUNNER_DIRECTORY})

#Clean Mock Directory
FILE(GLOB DATA_LAKE_MOCK_DIRECTORY_OLD_FILES "${DATA_LAKE_MOCK_DIRECTORY}/*")
IF(DATA_LAKE_MOCK_DIRECTORY_OLD_FILES)
	FILE(REMOVE ${DATA_LAKE_MOCK_DIRECTORY_OLD_FILES})
ENDIF()

#No dll.
ADD_DEFINITIONS(-DMCL_CORE_EXPORT=)

#Create mocks for headers in src folder.
SET(RUBY_CMD "ruby")
SET(RUBY_SCRIPT_PATH "mcl_core/test/lib/CMock/lib/cmock.rb")
SET(CMOCK_YML_FILE_INPUT "cmock.yml.in")
SET(CMOCK_YML_FILE "${CMAKE_CURRENT_BINARY_DIR}/cmock.yml")
SET(CMOCK_YML_INCLUDES "")
CONFIGURE_FILE(${CMOCK_YML_FILE_INPUT} ${CMOCK_YML_FILE})
FILE(GLOB HEADERS_TO_MOCK "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/src/*.h")
SET(MOCK_MCL_DATA_LAKE_H "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/include/mcl_data_lake/mcl_data_lake.h")
SET(MOCK_DATA_LAKE_PROCESSOR_H "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/src/data_lake_processor.h")

EXECUTE_PROCESS(COMMAND ${RUBY_CMD} ${RUBY_SCRIPT_PATH} -o${CMOCK_YML_FILE} ${HEADERS_TO_MOCK}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE ruby_result	
    OUTPUT_VARIABLE ruby_output)
	
SET(CORE_MOCK_DIRECTORY "${CMAKE_BINARY_DIR}/mcl_core/test/unit/mock")
FILE(GLOB MCL_CORE_INTERFACE_MOCK_FILES "${CORE_MOCK_DIRECTORY}/mock_mcl*.*")
FOREACH(MCL_CORE_INTERFACE_MOCK_FILE ${MCL_CORE_INTERFACE_MOCK_FILES})
	FILE(COPY ${MCL_CORE_INTERFACE_MOCK_FILE} DESTINATION ${DATA_LAKE_MOCK_DIRECTORY})
ENDFOREACH()

#Configure yml and create mock for headers in include folder. The mocks header will also include the corresponding header in src folder (e.g. to include typedef/structs).
FILE(GLOB INCLUDE_HEADERS_TO_MOCK "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/include/mcl_data_lake/*.h")
FOREACH(INCLUDE_HEADER_TO_MOCK ${INCLUDE_HEADERS_TO_MOCK})
    #only consider headers : include/mcl_data_lake/mcl_*.h
    IF(INCLUDE_HEADER_TO_MOCK MATCHES "^.*mcl_[a-zA-Z_]*\\.h$")
        STRING(REGEX REPLACE "^(.*)mcl_" "" SOURCE_HEADER_TO_INCLUDE ${INCLUDE_HEADER_TO_MOCK})
        #if src/*.h doesnt exist dont include in mock-header
        IF(EXISTS "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/src/${SOURCE_HEADER_TO_INCLUDE}")
            SET(CMOCK_YML_INCLUDES "[${SOURCE_HEADER_TO_INCLUDE}]")
        ELSE()
            SET(CMOCK_YML_INCLUDES "")
        ENDIF()
        #MESSAGE(STATUS "Creating mock for ${INCLUDE_HEADER_TO_MOCK} with include of ${SOURCE_HEADER_TO_INCLUDE}")
        CONFIGURE_FILE(${CMOCK_YML_FILE_INPUT} ${CMOCK_YML_FILE})
        EXECUTE_PROCESS(COMMAND ${RUBY_CMD} ${RUBY_SCRIPT_PATH} -o${CMOCK_YML_FILE} ${INCLUDE_HEADER_TO_MOCK}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            RESULT_VARIABLE ruby_result	
            OUTPUT_VARIABLE ruby_output)
     ENDIF()
ENDFOREACH()

#Add data_lake.h, data_lake_object.h and data_lake_configuration.h to mock_mcl_data_lake.h	
SET(CMOCK_YML_INCLUDES "[data_lake.h, data_lake_object.h, data_lake_configuration.h]")
CONFIGURE_FILE(${CMOCK_YML_FILE_INPUT} ${CMOCK_YML_FILE})
EXECUTE_PROCESS(COMMAND ${RUBY_CMD} ${RUBY_SCRIPT_PATH} -o${CMOCK_YML_FILE} ${MOCK_MCL_DATA_LAKE_H}
WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
RESULT_VARIABLE ruby_result	
OUTPUT_VARIABLE ruby_output)
    		
#Add data_lake_object.h to mock_data_lake_processor.h	
SET(CMOCK_YML_INCLUDES "[data_lake_object.h]")
CONFIGURE_FILE(${CMOCK_YML_FILE_INPUT} ${CMOCK_YML_FILE})
EXECUTE_PROCESS(COMMAND ${RUBY_CMD} ${RUBY_SCRIPT_PATH} -o${CMOCK_YML_FILE} ${MOCK_DATA_LAKE_PROCESSOR_H}
WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
RESULT_VARIABLE ruby_result	
OUTPUT_VARIABLE ruby_output)
    		
#Set test libraries
SET(TEST_LIBS ${TEST_FRAMEWORK})

#Set mock sources
FILE(GLOB DATA_LAKE_MOCK_SOURCES "${DATA_LAKE_MOCK_DIRECTORY}/*.c")

#Set mock library
SET(DATA_LAKE_MOCK_LIB DATA_LAKE_MOCK_SOURCES)
ADD_LIBRARY(${DATA_LAKE_MOCK_LIB} ${DATA_LAKE_MOCK_SOURCES})
TARGET_LINK_LIBRARIES(${DATA_LAKE_MOCK_LIB} ${TEST_LIBS})
TARGET_INCLUDE_DIRECTORIES(${DATA_LAKE_MOCK_LIB} PUBLIC ${MCL_DATA_LAKE_INCLUDE_DIRECTORIES} ${MCL_CORE_INCLUDE_DIRECTORIES} "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/include/mcl_data_lake" "${MCL_CMAKE_ROOT_DIR}/mcl_core/include/mcl_core")

#PIC option is required in order to build DATA_LAKE_MOCK_LIB with shared library target
SET_TARGET_PROPERTIES(${DATA_LAKE_MOCK_LIB} PROPERTIES POSITION_INDEPENDENT_CODE ON)

#Append mock library to test libraries
LIST(APPEND TEST_LIBS ${DATA_LAKE_MOCK_LIB})

#Loop over each unit test file.
FILE(GLOB UNIT_TEST_FILE_LIST RELATIVE "${TEST_CASE_DIRECTORY}" "${TEST_CASE_DIRECTORY}/*.c") 
FOREACH(UNIT_TEST_FILE ${UNIT_TEST_FILE_LIST})

    #Remove file extension from the testcase file
    STRING(REPLACE ".c" "" UNIT_TEST_FILE_NAME ${UNIT_TEST_FILE})
                 
    #Create test runner.
    SET(TEST_RUNNER_FILE "${UNIT_TEST_FILE_NAME}_runner.c")
    EXECUTE_PROCESS(COMMAND ${RUBY_CMD} ${GENERATE_TEST_RUNNER_SCRIPT_PATH} "${TEST_CASE_DIRECTORY}/${UNIT_TEST_FILE}" "${TEST_RUNNER_DIRECTORY}/${TEST_RUNNER_FILE}"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE ruby_result	
        OUTPUT_VARIABLE ruby_output)
    
    #Set original source files. 
    SET(ORIGINAL_SOURCES "")
    FILE(STRINGS "${TEST_CASE_DIRECTORY}/${UNIT_TEST_FILE}" INCLUDE_LINE_LIST REGEX "^#include*")     
    FOREACH(INCLUDE_LINE ${INCLUDE_LINE_LIST})
        STRING(REPLACE "#include \"" "" INCLUDE_FILE_H_ ${INCLUDE_LINE})
        STRING(REGEX REPLACE "\".*" "" INCLUDE_FILE_H_ ${INCLUDE_FILE_H_}) 
        STRING(REGEX REPLACE ".*/" "" INCLUDE_FILE_H_ ${INCLUDE_FILE_H_})
        STRING(REPLACE ".h" ".c" INCLUDE_FILE_C_ ${INCLUDE_FILE_H_})
        IF(EXISTS "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/src/${INCLUDE_FILE_H_}")
            LIST(REMOVE_ITEM DATA_LAKE_MOCK_SOURCES "${DATA_LAKE_MOCK_DIRECTORY}/mock_${INCLUDE_FILE_C_}")
            IF(EXISTS "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/src/${INCLUDE_FILE_C_}")
                LIST(APPEND ORIGINAL_SOURCES "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/src/${INCLUDE_FILE_C_}")
            ENDIF() 
        ELSEIF(EXISTS "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/include/mcl_data_lake/mcl_${INCLUDE_FILE_H_}")
            LIST(REMOVE_ITEM DATA_LAKE_MOCK_SOURCES "${DATA_LAKE_MOCK_DIRECTORY}/mock_${INCLUDE_FILE_C_}")
            IF(EXISTS "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/src/${INCLUDE_FILE_C_}")
                LIST(APPEND ORIGINAL_SOURCES "${MCL_DATA_LAKE_CMAKE_ROOT_DIR}/src/${INCLUDE_FILE_C_}")
            ENDIF()
        ENDIF()
    ENDFOREACH(INCLUDE_LINE)
	
	LIST(APPEND ORIGINAL_SOURCES ${MCL_CORE_UTILITY_SOURCE})

    #Set test source files.             
    SET(TEST_SOURCES ${TEST_CASE_DIRECTORY}/${UNIT_TEST_FILE} ${TEST_RUNNER_DIRECTORY}/${TEST_RUNNER_FILE})

    #Set the name of the test executable.   
    SET(UNIT_TEST_EXECUTABLE ${UNIT_TEST_FILE_NAME})

    #Create test executable.    
    ADD_EXECUTABLE(${UNIT_TEST_EXECUTABLE} $<TARGET_OBJECTS:cJSON> ${TEST_SOURCES} ${ORIGINAL_SOURCES})    
        
    #Link libraries to executable.
    TARGET_LINK_LIBRARIES(${UNIT_TEST_EXECUTABLE} ${TEST_LIBS})

	#Set linker flag -lm for linking against the math lib (pow() floor())
	IF(CMAKE_COMPILER_IS_GNUCC)
		TARGET_LINK_LIBRARIES(${UNIT_TEST_EXECUTABLE} m)
	ENDIF()

    #Include required directories
    TARGET_INCLUDE_DIRECTORIES(${UNIT_TEST_EXECUTABLE} PUBLIC ${DATA_LAKE_MOCK_DIRECTORY})
    TARGET_INCLUDE_DIRECTORIES(${UNIT_TEST_EXECUTABLE} PUBLIC ${MCL_DATA_LAKE_INCLUDE_DIRECTORIES})
	
	SET_TARGET_PROPERTIES(${UNIT_TEST_EXECUTABLE} PROPERTIES
		FOLDER "${MCL_COMPONENT}/${TEST_TYPE}_tests")

    ADD_TEST(${UNIT_TEST_EXECUTABLE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${UNIT_TEST_EXECUTABLE})
    
ENDFOREACH(UNIT_TEST_FILE)
