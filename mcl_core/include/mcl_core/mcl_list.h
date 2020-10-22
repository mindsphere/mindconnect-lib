/*!
 * @file     mcl_list.h
 * @brief    List module interface header file.
 *
 * The list interface provides #mcl_list_t data structure for linked lists
 * and functions for list operations.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_LIST_H_
#define MCL_LIST_H_

#include "mcl_core/mcl_core_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for list node.
 */
typedef struct mcl_list_node_t
{
    void *data;                      //!< Data of the node.
    struct mcl_list_node_t *prev;    //!< Previous node in the list.
    struct mcl_list_node_t *next;    //!< Next node in the list.
} mcl_list_node_t;

/**
 * Handle for list.
 */
typedef struct mcl_list_t
{
    mcl_list_node_t *head;           //!< Head node of the list.
    mcl_list_node_t *last;           //!< Last node of the list.
    mcl_list_node_t *current;        //!< Current node of the list.
    mcl_size_t count;                //!< Node count of the list.
} mcl_list_t;

/**
 * Callback function prototype to compare @p item_to_compare to the @p reference_item.
 *
 * @param [in] reference_item Reference item to compare to.
 * @param [in] item_to_compare Item to compare with the @p reference_item.
 * @return
 * <ul>
 * <li>#MCL_OK in case of match.</li>
 * <li>#MCL_FAIL in case of mismatch.</li>
 * </ul>
 */
typedef mcl_error_t (*mcl_list_compare_callback)(void *reference_item, const void *item_to_compare);

/**
 * Callback function prototype to destroy a list item.
 *
 * @param [in] item Address of pointer to the item.
 */
typedef void (*mcl_list_item_destroy_callback)(void **item);

/**
 * This function initializes a list with zero items in it.
 *
 * @param [out] list Initialized #mcl_list_t instance.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_list_initialize(mcl_list_t **list);

/**
 * This function adds a new item to list.
 *
 * A new list item pointed by @p data is added to the list. No new memory is allocated and no memory copy operations done.
 * The lifetime of @p data should be handled by the caller of this function.
 *
 * @param [in] list The list to which the data is added.
 * @param [in] data The pointer to the list item to be added.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_LIMIT_EXCEEDED in case @p list is full and @p data can not be added to @p list.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_list_add(mcl_list_t *list, void *data);

/**
 * This function gets the next node from the list.
 *
 * Last returned node is kept in list. This function can be called consequently to loop over the list.
 * If there is no node left to return or the list is empty, @p node will be NULL.
 *
 * @param [in] list The list.
 * @param [out] node Address of next node.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
 * <li>#MCL_FAIL if there is no node in the @p list.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_list_next(mcl_list_t *list, mcl_list_node_t **node);

/**
 * This function removes a node from the list.
 *
 * The list node is removed from the list and its handle of type #mcl_list_node_t will be freed but the data that it holds will not.
 * <b>User needs to free the resource before it calls remove!</b>
 *
 * @param [in] list The list from which the node is removed.
 * @param [in] node The node to be removed.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
 * <li>#MCL_LIST_IS_EMPTY in case @p list is empty.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_list_remove(mcl_list_t *list, mcl_list_node_t *node);

/**
 * This function removes a node from the list and destroys the removed item with the provided callback function.
 *
 * The node (#mcl_list_node_t) will be freed and the data that it holds is going to be destroyed by passing it to the provided callback function.
 *
 * @param [in] list The list from which the node is removed.
 * @param [in] node The node to be removed.
 * @param [in] callback The callback function to destroy the list item.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
 * <li>#MCL_LIST_IS_EMPTY in case @p list is empty.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_list_remove_with_content(mcl_list_t *list, mcl_list_node_t *node, mcl_list_item_destroy_callback callback);

/**
 * This function searches @p item_to_find in the @p list.
 *
 * @param [in] list The list to search in.
 * @param [in] item_to_find Item to search for.
 * @param [in] compare_function Callback function used to compare.
 * @param [out] item Pointer to the item found.
 * @return
 * <ul>
 * <li>#MCL_OK if @p item_to_find exists in the @p list.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
 * <li>#MCL_FAIL if @p item_to_find doesn't exist in the @p list.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_list_exist(mcl_list_t *list, const void *item_to_find, mcl_list_compare_callback compare_function, void **item);

/**
 * This function resets the current node to head node.
 *
 * The goal here is to be able to loop over the list from it's beginning.
 *
 * @param [in] list The list which is used to reset it's current node to it's head node.
 */
extern MCL_CORE_EXPORT void mcl_list_reset(mcl_list_t *list);

/**
 * This function destroys the list.
 *
 * For every node in the list, it frees the node but not the data it holds. After all nodes freed,
 * it frees the list itself and sets it's value to NULL.
 *
 * <b>User needs to free all the data that the list is holding before destroying it! </b>
 *
 * @param [in] list The address of the pointer of the list to be destroyed.
 */
extern MCL_CORE_EXPORT void mcl_list_destroy(mcl_list_t **list);

/**
 * This function destroys the list and its items with a given callback function.
 *
 * For every node in the list, it frees the node. The data of the node can also freed by given callback function. After all nodes freed,
 * it frees the list itself and sets it's value to NULL.
 *
 * @param [in] list The address of the pointer of the list to be destroyed.
 * @param [in] callback The callback function to destroy each node data within the list. If NULL no action will be performed.
 */
extern MCL_CORE_EXPORT void mcl_list_destroy_with_content(mcl_list_t **list, mcl_list_item_destroy_callback callback);

#ifdef  __cplusplus
}
#endif

#endif //MCL_LIST_H_
