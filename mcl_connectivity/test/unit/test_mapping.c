/*!
 * @file     test_mapping.c
 * @brief    This file implements all test cases of mcl_mapping module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_mapping.h"
#include "mapping.h"
#include "mcl_core/mcl_log_util.h"
#include "mcl_core/mcl_json_util.h"
#include "mcl_core/mcl_memory.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_mapping_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized mapping struct.
 */
void test_initialize_001(void)
{
    mcl_mapping_t *mapping = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_mapping_initialize(&mapping);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for mapping");
    TEST_ASSERT_NOT_NULL_MESSAGE(mapping, "mapping is NULL");

    // Clean up.
    mcl_mapping_destroy(&mapping);
}

/**
 * GIVEN : mcl_mapping parameter is given as MCL_NULL.
 * WHEN  : mcl_mapping_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    mcl_error_t code;

    // Test.
    code = mcl_mapping_initialize(MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_mapping_initialize() unexpected return error.");
}

/**
 * GIVEN : Mapping parameter is NULL.
 * WHEN  : mcl_mapping_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001()
{
    const char *dummy_value = "dummy";
    mcl_error_t code = mcl_mapping_set_parameter(MCL_NULL, MCL_MAPPING_PARAMETER_DATA_POINT_ID, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_mapping_set_parameter() failed when mapping is NULL.");
}

/**
 * GIVEN : Value parameter is NULL.
 * WHEN  : mcl_mapping_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002()
{
    mcl_mapping_t mapping;
    mcl_error_t code = mcl_mapping_set_parameter(&mapping, MCL_MAPPING_PARAMETER_DATA_POINT_ID, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_mapping_set_parameter() failed when value is NULL.");
}

/**
 * GIVEN : Invalid parameter.
 * WHEN  : mcl_mapping_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_003()
{
    mcl_mapping_t mapping;
    const char *dummy_value = "dummy";
    mcl_error_t code = mcl_mapping_set_parameter(&mapping, MCL_MAPPING_PARAMETER_END, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_mapping_set_parameter() failed when parameter is invalid.");
}

/**
 * GIVEN : Data point id parameter.
 * WHEN  : mcl_mapping_set_parameter() is called.
 * THEN  : MCL_OK is returned and data point id parameter of the mapping is set.
 */
void test_set_parameter_004()
{
    // Initialize mapping.
    mcl_mapping_t *mapping;
    mcl_error_t code = mcl_mapping_initialize(&mapping);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for mapping");

    // Set parameter.
    const char *data_point_id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    code = mcl_mapping_set_parameter(mapping, MCL_MAPPING_PARAMETER_DATA_POINT_ID, data_point_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_mapping_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_id, mapping->data_point_id, "Data point ID parameter is wrong.");

    // Clean up.
    mcl_mapping_destroy(&mapping);
}

/**
 * GIVEN : Entity id parameter.
 * WHEN  : mcl_mapping_set_parameter() is called.
 * THEN  : MCL_OK is returned and entity id parameter of the mapping is set.
 */
void test_set_parameter_005()
{
    // Initialize mapping.
    mcl_mapping_t *mapping;
    mcl_error_t code = mcl_mapping_initialize(&mapping);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for mapping");

    // Set parameter.
    const char *entity_id = "dummy_entity_id";
    code = mcl_mapping_set_parameter(mapping, MCL_MAPPING_PARAMETER_ENTITY_ID, entity_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_mapping_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(entity_id, mapping->entity_id, "Entity ID parameter is wrong.");

    // Clean up.
    mcl_mapping_destroy(&mapping);
}

/**
 * GIVEN : Property set name parameter.
 * WHEN  : mcl_mapping_set_parameter() is called.
 * THEN  : MCL_OK is returned and property set name parameter of the mapping is set.
 */
void test_set_parameter_006()
{
    // Initialize mapping.
    mcl_mapping_t *mapping;
    mcl_error_t code = mcl_mapping_initialize(&mapping);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for mapping");

    // Set parameter.
    const char *property_set_name = "ElectricalProperties";
    code = mcl_mapping_set_parameter(mapping, MCL_MAPPING_PARAMETER_PROPERTY_SET_NAME, property_set_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_mapping_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(property_set_name, mapping->property_set_name, "Property set name parameter is wrong.");

    // Clean up.
    mcl_mapping_destroy(&mapping);
}

/**
 * GIVEN : Property name parameter.
 * WHEN  : mcl_mapping_set_parameter() is called.
 * THEN  : MCL_OK is returned and property name parameter of the mapping is set.
 */
void test_set_parameter_007()
{
    // Initialize mapping.
    mcl_mapping_t *mapping;
    mcl_error_t code = mcl_mapping_initialize(&mapping);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for mapping");

    // Set parameter.
    const char *property_name = "uint";
    code = mcl_mapping_set_parameter(mapping, MCL_MAPPING_PARAMETER_PROPERTY_NAME, property_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_mapping_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(property_name, mapping->property_name, "Property name parameter is wrong.");

    // Clean up.
    mcl_mapping_destroy(&mapping);
}

/**
 * GIVEN : Keep mapping parameter.
 * WHEN  : mcl_mapping_set_parameter() is called.
 * THEN  : MCL_OK is returned and keep mapping parameter of the mapping is set.
 */
void test_set_parameter_008()
{
    // Initialize mapping.
    mcl_mapping_t *mapping;
    mcl_error_t code = mcl_mapping_initialize(&mapping);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for mapping");

    // Set parameter.
    mcl_bool_t keep_mapping = MCL_FALSE;
    code = mcl_mapping_set_parameter(mapping, MCL_MAPPING_PARAMETER_KEEP_MAPPING, &keep_mapping);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_mapping_set_parameter() failed.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(keep_mapping, mapping->keep_mapping, "Keep mapping parameter is wrong.");

    // Clean up.
    mcl_mapping_destroy(&mapping);
}

/**
 * GIVEN : Mandatory mapping parameters.
 * WHEN  : mapping_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_mapping_validate_001()
{
    // Initialize mapping.
    mcl_mapping_t *mapping;
    mcl_error_t code = mcl_mapping_initialize(&mapping);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for mapping");

    // Set data point id parameter.
    const char *data_point_id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    code = mcl_mapping_set_parameter(mapping, MCL_MAPPING_PARAMETER_DATA_POINT_ID, data_point_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_mapping_set_parameter() failed.");

    // Set property set name parameter.
    const char *property_set_name = "ElectricalProperties";
    code = mcl_mapping_set_parameter(mapping, MCL_MAPPING_PARAMETER_PROPERTY_SET_NAME, property_set_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_mapping_set_parameter() failed.");
    
    // Set property name parameter.
    const char *property_name = "Voltage";
    code = mcl_mapping_set_parameter(mapping, MCL_MAPPING_PARAMETER_PROPERTY_NAME, property_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_mapping_set_parameter() failed.");

    // Test.
    code = mapping_validate(mapping);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mapping_validate() failed.");

    // Clean up.
    mcl_mapping_destroy(&mapping);
}

/**
 * GIVEN : Mandatory mapping parameters are not set.
 * WHEN  : mapping_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_mapping_validate_002()
{
    // Initialize mapping.
    mcl_mapping_t *mapping;
    mcl_error_t code = mcl_mapping_initialize(&mapping);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for mapping");

    // Test.
    code = mapping_validate(mapping);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mapping_validate() failed when mandatory parameters are NULL.");

    // Clean up.
    mcl_mapping_destroy(&mapping);
}
