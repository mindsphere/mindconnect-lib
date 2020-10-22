/*!
 * @file     test_list.c
 * @brief    Unit test cases for testing list module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_core/mcl_list.h"
#include "memory.h"
#include "string_util.h"
#include "mcl_core/mcl_core_common.h"
#include "mcl_core/mcl_log_util.h"

mcl_error_t _string_compare_with_cstr(char *string, const char *other);

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Nothing.
 * WHEN  : mcl_list_initialize() is called.
 * THEN  : List is initialized and MCL_OK is returned.
 */
void test_initialize_001(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Check initialized list.
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "List couldn't be initialized!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, list->count, "Count of list is not 0!");
    TEST_ASSERT_NULL_MESSAGE(list->head, "Head node of list is not NULL!");
    TEST_ASSERT_NULL_MESSAGE(list->last, "Last node of list is not NULL!");
    TEST_ASSERT_NULL_MESSAGE(list->current, "Current node of list is not NULL!");

    // Clean up.
    mcl_list_destroy(&list);
}

/**
 * GIVEN : Initialized empty list.
 * WHEN  : mcl_list_next() is called.
 * THEN  : Received node is NULL and MCL_FAIL is returned.
 */
void test_next_001(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Call next node in the list.
    mcl_list_node_t *node;
    code = mcl_list_next(list, &node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_FAIL, code, "Next operation failed!");
    TEST_ASSERT_NULL_MESSAGE(node, "mcl_list_next() on empty list returned not NULL node!");

    // Check everything is still NULL.
    TEST_ASSERT_NULL_MESSAGE(list->head, "Head node of list is not NULL!");
    TEST_ASSERT_NULL_MESSAGE(list->last, "Last node of list is not NULL!");
    TEST_ASSERT_NULL_MESSAGE(list->current, "Current node of list is not NULL!");

    // Clean up.
    mcl_list_destroy(&list);
}

/**
 * GIVEN : List with 2 nodes in it.
 * WHEN  : mcl_list_next() is called.
 * THEN  : Next node is received.
 */
void test_next_002(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Add first value to the list.
    int *value_1 = (int *) 5;
    code = mcl_list_add(list, value_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first value operation failed!");

    // Add second value to the list.
    int *value_2 = (int *) 10;
    code = mcl_list_add(list, value_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding second value operation failed!");

    // Make sure the current node is the head node.
    mcl_list_reset(list);
    TEST_ASSERT_MESSAGE(list->head == list->current, "List's current node couldn't be reset to head node.");

    // After first call of mcl_list_next(), check if the next node is the current node.
    mcl_list_node_t *next_node;
    code = mcl_list_next(list, &next_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "First next operation failed!");
    TEST_ASSERT_EQUAL_MESSAGE(value_1, next_node->data, "Item of first node is not correct.");

    // After second call of mcl_list_next(), check if the next node is the second node.
    code = mcl_list_next(list, &next_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Second next operation failed!");
    TEST_ASSERT_EQUAL_MESSAGE(value_2, next_node->data, "Item of second node is not correct.");

    // After third call of mcl_list_next(), check if the next node is the second node.
    code = mcl_list_next(list, &next_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_FAIL, code, "Third next operation failed!");
    TEST_ASSERT_NULL_MESSAGE(next_node, "Next node received as not NULL!");

    // After fourth call of mcl_list_next(), check if the next node is the first node.
    code = mcl_list_next(list, &next_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Fourth next operation failed!");
    TEST_ASSERT_EQUAL_MESSAGE(value_1, next_node->data, "Item of first node is not correct.");

    // Clean up.
    mcl_list_destroy(&list);
}

/**
 * GIVEN : Initialized list and a new item to be added to the list.
 * WHEN  : mcl_list_add() is called.
 * THEN  : Item is added to list and MCL_OK is returned.
 */
void test_add_001(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Add item to the list.
    int *value = (int *) 5;
    code = mcl_list_add(list, value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Add operation failed!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "Count of list is not 1!");

    // Get the next node.
    mcl_list_node_t *next_node = MCL_NULL;
    code = mcl_list_next(list, &next_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Next operation failed!");
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node, "Next node received as NULL!");
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node->data, "Item in received next node is NULL!");

    // Get the item of next node.
    int *value_from_list = (int *) next_node->data;
    TEST_ASSERT_EQUAL_MESSAGE(value, value_from_list, "Received value from list is not as expected!");

    // Clean up.
    mcl_list_destroy(&list);
}

/**
 * GIVEN : Initialized list and multiple new items to be added to the list.
 * WHEN  : mcl_list_add() is called.
 * THEN  : All items are added to list and MCL_OK is returned.
 */
void test_add_002(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Add 10 values to the list.
    int *value = (int *) 5;
    int count = 10;

    for (int i = 0; i < count; i++)
    {
        code = mcl_list_add(list, value);
        TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Add operation failed!");
    }

    // Check values count in the list.
    TEST_ASSERT_EQUAL_INT_MESSAGE(count, list->count, "Count of list is not as expected!");

    // Clean up.
    mcl_list_destroy(&list);
}

/**
 * GIVEN : List with a node in it.
 * WHEN  : mcl_list_remove() is called.
 * THEN  : The specified node is removed from the list and MCL_OK is returned.
 */
void test_remove_001(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Create a value to add to the list.
    char *value = string_util_strdup("value");

    // Add the value to the list.
    code = mcl_list_add(list, value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Add operation failed!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "Count of list is not 1!");

    // Get the next node.
    mcl_list_node_t *next_node;
    code = mcl_list_next(list, &next_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Next operation failed!");
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node, "Next node received as NULL!");
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node->data, "Item in received next node is NULL!");

    // Remove the node.
    code = mcl_list_remove(list, next_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Remove operation failed!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, list->count, "Count of list is not 0!");
    TEST_ASSERT_NULL_MESSAGE(list->current, "Current node is not NULL!");
    TEST_ASSERT_NULL_MESSAGE(list->head, "Head node is not NULL!");
    TEST_ASSERT_NULL_MESSAGE(list->last, "Last node is not NULL!");

    // Make sure the item is not freed.
    TEST_ASSERT_NOT_NULL_MESSAGE(value, "The item shouldn't have been NULL!");

    // Clean up.
    mcl_list_destroy(&list);
    MCL_FREE(value);
}

/**
 * GIVEN : List with a node in it.
 * WHEN  : mcl_list_remove() is called.
 * THEN  : The specified node is removed but it's item is still accessible and MCL_OK is returned.
 */
void test_remove_002(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Create a value to add to the list.
    char *value = string_util_strdup("value");

    // Add the value to the list.
    code = mcl_list_add(list, value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Add operation failed!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "Count of list is not 1!");

    // Get the next node.
    mcl_list_node_t *next_node = MCL_NULL;
    mcl_list_next(list, &next_node);
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node, "Next node received as NULL!");
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node->data, "Item in received next node is NULL!");

    // Keep the item address before removing the node in the list.
    char *item = (char *) next_node->data;

    // Remove the node.
    code = mcl_list_remove(list, next_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Remove operation failed!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, list->count, "Count of list is not 0!");
    TEST_ASSERT_EQUAL_MESSAGE(value, item, "The item of removed node is not accessible!");

    // Clean up.
    mcl_list_destroy(&list);
    MCL_FREE(value);
}

/**
 * GIVEN : List with 10 nodes in it.
 * WHEN  : mcl_list_remove() is called 10 times.
 * THEN  : All 10 nodes are removed from the list and MCL_OK is returned.
 */
void test_remove_003(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Add 10 values to the list.
    int *value = (int *) 5;
    int count = 10;

    for (int i = 0; i < count; i++)
    {
        code = mcl_list_add(list, value);
        TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Add operation failed!");
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(count, list->count, "Count of list is not as expected!");

    // Get the next node.
    mcl_list_node_t *next_node = MCL_NULL;
    int index = 0;
    mcl_list_next(list, &next_node);

    while (MCL_NULL != next_node)
    {
        // Check the next node.
        TEST_ASSERT_NOT_NULL_MESSAGE(next_node, "Next node received as NULL!");
        TEST_ASSERT_NOT_NULL_MESSAGE(next_node->data, "Item in received next node is NULL!");

        // Remove the node from the list.
        code = mcl_list_remove(list, next_node);
        TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Remove operation failed!");

        // Get the next node.
        mcl_list_next(list, &next_node);

        // Increase the index.
        index++;
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, list->count, "After all remove operations, count of list is not 0!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(count, index, "Remoeved item number is different from added item number.");

    // Clean up.
    mcl_list_destroy(&list);
}

/**
 * GIVEN : List with a node in it.
 * WHEN  : mcl_list_remove_with_content() is called.
 * THEN  : In addition to removing the node, it frees the memory allocated for the item data. And MCL_OK is returned.
 */
void test_remove_with_content_001(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Initialize list as node.
    mcl_list_t *list_as_node = MCL_NULL;
    code = mcl_list_initialize(&list_as_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List as node couldn't be initialized.");

    // Add list as node to the list.
    code = mcl_list_add(list, list_as_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Add operation failed!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "Count of list is not 1!");

    // Get the next node.
    mcl_list_node_t *next_node;
    code = mcl_list_next(list, &next_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Next operation failed!");
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node, "Next node received as NULL!");
    TEST_ASSERT_NOT_NULL_MESSAGE(next_node->data, "Item in received next node is NULL!");

    // Remove the list with content.
    code = mcl_list_remove_with_content(list, next_node, (mcl_list_item_destroy_callback)mcl_list_destroy);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Remove operation failed!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, list->count, "Count of list is not 0!");

    // Clean up.
    mcl_list_destroy(&list);
}

/**
 * GIVEN : List with 10 nodes in it.
 * WHEN  : mcl_list_reset() is called.
 * THEN  : Resets the current node to head node.
 */
void test_reset_001(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Add 10 values to the list.
    int *value = (int *) 5;
    int count = 10;

    for (int i = 0; i < count; i++)
    {
        code = mcl_list_add(list, value);
        TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Add operation failed!");
    }

    // Make sure current node is not the head node by calling mcl_list_next().
    mcl_list_node_t *next_node;
    code = mcl_list_next(list, &next_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Next operation failed!");

    // Make the current node second node in the list.
    code = mcl_list_next(list, &next_node);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Next operation failed!");

    // Before reset the list, verify current node is not the head node.
    TEST_ASSERT_MESSAGE(list->head != list->current, "Current node shouldn't have been head node.");
    mcl_list_reset(list);

    // After reset the list, verify current node is the head node.
    TEST_ASSERT_MESSAGE(list->head == list->current, "List's current node couldn't be reset to head node.");

    // Clean up.
    mcl_list_destroy(&list);
}

/**
 * GIVEN : List with the item to find in the list.
 * WHEN  : mcl_list_exist() is called.
 * THEN  : The item to find is returned.
 */
void test_exist_001(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Initialize value.
    char *value = string_util_strdup("value1");

    // Add value to the list.
    code = mcl_list_add(list, value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Add operation failed!");

    // Check if the item exists or not.
    void *node_data_of_item = MCL_NULL;
    code = mcl_list_exist(list, value, (mcl_list_compare_callback) _string_compare_with_cstr, &node_data_of_item);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_list_exist() operation failed!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(value, ((char *) node_data_of_item), "Received data_point_id is not as expected!");

    // Clean up.
    mcl_list_destroy(&list);
    MCL_FREE(value);
}

/**
 * GIVEN : List without the item to find in the list.
 * WHEN  : mcl_list_exist() is called.
 * THEN  : MCL_FAIL is returned and node_data_of_item becomes null.
 */
void test_exist_002(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Initialize values.
    char *value1 = string_util_strdup("value1");
    char *value2 = string_util_strdup("value2");

    // Add value to the list.
    code = mcl_list_add(list, value1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Add operation failed!");

    // Check if the item exists or not.
    void *node_data_of_item = MCL_NULL;
    code = mcl_list_exist(list, value2, (mcl_list_compare_callback)_string_compare_with_cstr, &node_data_of_item);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_FAIL, code, "mcl_list_exist operation didn't return MCL_FAIL!");
    TEST_ASSERT_NULL_MESSAGE(node_data_of_item, "node_data_of_item is not null.");

    // Clean up.
    mcl_list_destroy(&list);
    MCL_FREE(value1);
    MCL_FREE(value2);
}

/**
 * GIVEN : Initialized list with two items in it.
 * WHEN  : mcl_list_destroy() is called.
 * THEN  : The list is freed and pointer is NULL but the contents are not freed.
 */
void test_destroy_001(void)
{
    // Initialize list.
    mcl_list_t *list = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List couldn't be initialized.");

    // Initialize values.
    char *values[2] = {0};

    values[0] = string_util_strdup("value_1");
    values[1] = string_util_strdup("value_2");

    // Add value to the list.
    int count = 2;

    for (int i = 0; i < count; i++)
    {
        code = mcl_list_add(list, values[i]);
        TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Add operation failed!");
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(count, list->count, "Count of list is not as expected!");

    // Destroy the list.
    mcl_list_destroy(&list);
    TEST_ASSERT_NULL_MESSAGE(list, "After destroy operation, list is not NULL!");

    // Make sure the items are not freed.
    TEST_ASSERT_NOT_NULL_MESSAGE(values[0], "values[0] shouldn't have been freed.");
    TEST_ASSERT_NOT_NULL_MESSAGE(values[1], "values[1] shouldn't have been freed.");

    // Clean up.
    mcl_list_destroy(&list);
    MCL_FREE(values[0]);
    MCL_FREE(values[1]);
}

/**
 * GIVEN : Initialized list.
 * WHEN  : mcl_list_destroy_with_content() is called.
 * THEN  : The list is freed with it's content and pointer is NULL.
 */
void test_destroy_with_content_001(void)
{
    // Initialize list_1.
    mcl_list_t *list_1 = MCL_NULL;
    mcl_error_t code = mcl_list_initialize(&list_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List 1 couldn't be initialized.");

    // Initialize list_2.
    mcl_list_t *list_2 = MCL_NULL;
    code = mcl_list_initialize(&list_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List 2 couldn't be initialized.");

    // Initialize list_of_lists.
    mcl_list_t *list_of_lists = MCL_NULL;
    code = mcl_list_initialize(&list_of_lists);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "List of lists couldn't be initialized.");

    // Add list_1 to list_of_lists.
    code = mcl_list_add(list_of_lists, list_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding list 1 operation failed!");

    // Add list_2 to list_of_lists.
    code = mcl_list_add(list_of_lists, list_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding list 2 operation failed!");
    TEST_ASSERT_EQUAL_MESSAGE(2, list_of_lists->count, "List does not contain 2 nodes.");

    // Destroy the list_of_lists with contents.
    mcl_list_destroy_with_content(&list_of_lists, (mcl_list_item_destroy_callback)mcl_list_destroy);
    TEST_ASSERT_NULL(list_of_lists);
}

mcl_error_t _string_compare_with_cstr(char *string, const char *other)
{
    mcl_error_t result = MCL_FAIL;
    mcl_size_t other_length = 0;
    mcl_size_t string_length = 0;

    MCL_DEBUG_ENTRY("char *string = <%p>, const char *other = <%s>", string, other);

    if (MCL_NULL != string)
    {
        if (MCL_NULL != other)
        {
            // First check if both are pointing the same memory space.
            if (string == other)
            {
                MCL_DEBUG("String and other are pointing to the same memory space.");
                result = MCL_OK;
            }
            else
            {
                other_length = string_util_strlen(other);
                string_length = string_util_strlen(string);
                if (other_length == string_length)
                {
                    result = string_util_strncmp(string, other, string_length);
                }
            }
        }
        else
        {
            MCL_DEBUG("Received compare string (other) is NULL.");
        }
    }
    else
    {
        MCL_DEBUG("Received string object or it's buffer is NULL");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}
