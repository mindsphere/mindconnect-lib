/*!
 * @file     list.c
 * @brief    List module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "list.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"

mcl_error_t mcl_list_initialize(mcl_list_t **list)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_list_t **list = <%p>", list);

    MCL_ASSERT_NOT_NULL(list, code);
    MCL_NEW(*list);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != *list, MCL_OUT_OF_MEMORY, "Not enough memory to allocate new list!");

    // Initialize the list.
    (*list)->count = 0;
    (*list)->current = MCL_NULL;
    (*list)->head = MCL_NULL;
    (*list)->last = MCL_NULL;

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_list_add(mcl_list_t *list, void *data)
{
    mcl_list_node_t *new_node;
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_list_t *list = <%p>, void *data = <%p>", list, data);

    MCL_ASSERT_NOT_NULL(list, code);
    MCL_ASSERT_NOT_NULL(data, code);

    MCL_ASSERT_CODE_MESSAGE(list->count < MCL_SIZE_MAX, MCL_LIMIT_EXCEEDED, "Index of the list is already at the maximum value. Not adding the new data!");

    MCL_NEW(new_node);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != new_node, MCL_OUT_OF_MEMORY, "Not enough memory to allocate new mcl_node!");

    new_node->data = data;
    new_node->prev = list->last;
    new_node->next = MCL_NULL;
    MCL_VERBOSE("new_node initialized and its data assigned to the received one. Node's data address = <%p>", (void *) new_node->data);

    if (0 == list->count)
    {
        MCL_VERBOSE("This is the first node in the list.");
        list->head = new_node;
        list->last = new_node;
        list->current = new_node;
    }
    else
    {
        list->last->next = new_node;
        list->last = new_node;
    }

    // If we are here, it means adding is successful, we can increase the count.
    ++list->count;
    MCL_VERBOSE("new_node has been added to the list. Current list count = <%d>", list->count);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_list_next(mcl_list_t *list, mcl_list_node_t **node)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_list_t *list = <%p>, mcl_list_node_t **node = <%p>", list, node);
    
    // Null check for node.
    MCL_ASSERT_NOT_NULL(node, code);
    *node = MCL_NULL;

    // Null check for list.
    MCL_ASSERT_NOT_NULL(list, code);

    *node = list->current;

    if (MCL_NULL == list->current)
    {
        // If current node was last element, return head.
        list->current = list->head;
    }
    else
    {
        // Set current node to next node for next call.
        list->current = list->current->next;
    }

    // Check if returned node is null.
    if (MCL_NULL != *node)
    {
        code = MCL_OK;
    }
    else
    {
        code = MCL_FAIL;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_list_remove(mcl_list_t *list, mcl_list_node_t *node)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_list_t *list = <%p>, mcl_list_node_t *node = <%p>", list, node);

    MCL_ASSERT_NOT_NULL(list, code);
    MCL_ASSERT_NOT_NULL(node, code);
    MCL_ASSERT_CODE_MESSAGE(list->count > 0, MCL_LIST_IS_EMPTY, "The list count is 0. Not removing any item!");

    // Node connection handling.
    MCL_VERBOSE("node connection handling begins");
    if (MCL_NULL == node->prev)
    {
        // First node.
        MCL_VERBOSE("node->prev is MCL_NULL.");
        MCL_VERBOSE("This is the first node.");

        if (MCL_NULL == node->next)
        {
            // Prev and next are MCL_NULL. This is the only node in list.
            // No need to handle any node connection.
            MCL_VERBOSE("node->next is NULL.");
            MCL_VERBOSE("This is the only node in list. No need for any connection handling.");
        }
        else
        {
            // Prev is MCL_NULL but next is not. This should be the last node.
            // Set MCL_NULL to (node->next)->prev.
            node->next->prev = MCL_NULL;
            MCL_VERBOSE("node->next is not MCL_NULL.");
            MCL_VERBOSE("There is node's after this node. Next node's prev is set NULL.");
        }
    }
    else
    {
        // Not the first node.
        MCL_VERBOSE("node->prev is not NULL.");
        MCL_VERBOSE("This is NOT the first node.");

        if (MCL_NULL == node->next)
        {
            // Prev is NOT MCL_NULL but next is MCL_NULL. This is the last node.
            // Set node->prev->next to MCL_NULL.
            node->prev->next = MCL_NULL;

            MCL_VERBOSE("node->next is MCL_NULL.");
            MCL_VERBOSE("This is the last node. Previous node's next is set NULL");
        }
        else
        {
            // Prev is NOT MCL_NULL, next is NOT MCL_NULL. This is a middle node.
            // Set node->prev->next to node->next and the other way.
            node->prev->next = node->next;
            node->next->prev = node->prev;
            MCL_VERBOSE("node->next and node->prev are not MCL_NULL.");
            MCL_VERBOSE("This is a middle node. prev's next is connected to current next. next's prev is connected to current prev.");
        }
    }

    MCL_VERBOSE("node connection handling ends");
    MCL_VERBOSE("list handling begins");

    // List handling.
    if (list->head == node)
    {
        // If this was the head node, take the head to the next one. If the next one is MCL_NULL then list->head will also be MCL_NULL. No need to check for MCL_NULL.
        list->head = node->next;
        MCL_VERBOSE("This is the head node. Head pointer updated to the next node");
    }

    if (list->last == node)
    {
        // If this is the last node take the last node to the previous one. If the prev one is MCL_NULL then list->prev will also be MCL_NULL. No need to check for MCL_NULL.
        list->last = node->prev;
        MCL_VERBOSE("This is the last node. Last pointer updated to the previous node");
    }

    if (list->current == node)
    {
        // If current currently points to this node, we slide it to the next.
        list->current = node->next;
        MCL_VERBOSE("This is the list's current node. Current pointer updated to the next node");
    }

    // Decrease the list count.
    --list->count;
    MCL_VERBOSE("List counter decreased by one. Current count = <%d>", list->count);
    MCL_VERBOSE("list handling ends");

    // Now node connection handling and list handling is completed. we can free the node.
    MCL_FREE(node);
    MCL_VERBOSE("node is freed");

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_list_remove_with_content(mcl_list_t *list, mcl_list_node_t *node, mcl_list_item_destroy_callback callback)
{
    mcl_error_t code;
    void *data;

    MCL_DEBUG_ENTRY("mcl_list_t *list = <%p>, mcl_list_node_t *node = <%p>, mcl_list_item_destroy_callback callback = <%p>", list, node, callback);

    MCL_ASSERT_NOT_NULL(list, code);
    MCL_ASSERT_NOT_NULL(node, code);
    MCL_ASSERT_NOT_NULL(callback, code);

    data = node->data;

    code = mcl_list_remove(list, node);

    if ((MCL_OK == code) && (MCL_NULL != data))
    {
        callback(&data);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_list_exist(mcl_list_t *list, const void *item_to_find, mcl_list_compare_callback compare_function, void **item)
{
    mcl_list_node_t *current_node;
    mcl_error_t code = MCL_FAIL;

    MCL_DEBUG_ENTRY("mcl_list_t *list = <%p>, const void *item_to_find = <%p>, mcl_list_compare_callback compare_function = <%p>, void **item = <%p>", list, item_to_find, compare_function, item);

    MCL_ASSERT_NOT_NULL(list, code);
    MCL_ASSERT_NOT_NULL(item_to_find, code);
    MCL_ASSERT_NOT_NULL(compare_function, code);
    MCL_ASSERT_NOT_NULL(item, code);

    // We don't call list_reset() not to change list's configuration of current node, next node etc.
    current_node = list->head;
    while (MCL_NULL != current_node)
    {
        mcl_error_t item_exists_in_the_list = compare_function(current_node->data, item_to_find);

        if (MCL_OK == item_exists_in_the_list)
        {
            *item = current_node->data;
            MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
            return MCL_OK;
        }

        // We don't call list_next() not to change list's configuration of current node, next node etc.
        current_node = current_node->next;
    }

    *item = MCL_NULL;

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_list_reset(mcl_list_t *list)
{
    MCL_DEBUG_ENTRY("mcl_list_t *list = <%p>", list);

    if (MCL_NULL != list)
    {
        list->current = list->head;
        MCL_VERBOSE("List is reset.");
    }
    else
    {
        MCL_VERBOSE("List is NULL.");
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

void mcl_list_destroy(mcl_list_t **list)
{
    MCL_DEBUG_ENTRY("mcl_list_t **list = <%p>", list);

    mcl_list_destroy_with_content(list, MCL_NULL);

    MCL_DEBUG_LEAVE("retVal = void");
}

void mcl_list_destroy_with_content(mcl_list_t **list, mcl_list_item_destroy_callback callback)
{
    MCL_DEBUG_ENTRY("mcl_list_t **list = <%p>, mcl_list_item_destroy_callback callback = <%p>", list, callback);

    if ((MCL_NULL != list) && (MCL_NULL != *list))
    {
        mcl_list_node_t *current_node = (*list)->head;

        MCL_VERBOSE("List count = <%d>", (*list)->count);

        while (MCL_NULL != current_node)
        {
            mcl_list_node_t *node_to_free;

            // We are not freeing the node->data if callback is NULL! It's users responsibility.
            if ((MCL_NULL != callback) && (MCL_NULL != current_node->data))
            {
                callback(&(current_node->data));
            }

            node_to_free = current_node;
            current_node = node_to_free->next;
            MCL_FREE(node_to_free);
        }

        MCL_FREE(*list);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
