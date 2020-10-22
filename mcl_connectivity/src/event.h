/*!
 * @file     event.h
 * @brief    Event module header file.
 *
 * Meta and payload parts of event type are initialized and filled in this module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "item.h"
#include "mcl_core/mcl_json_util.h"
#include "mcl_core/mcl_time_util.h"

/**
 * Handle for event payload.
 */
typedef struct event_payload_t
{
    char *id;                               //!< Unique identifier of the event.
    char *correlation_id;                   //!< Parent event id.
    char timestamp[MCL_TIMESTAMP_LENGTH];   //!< Creation time of the event in ISO format.
    mcl_int32_t severity;                   //!< Severity level.
    char *description;                      //!< Event description.
    char *type;                             //!< Type of the event.
    char *version;                          //!< Version of the event/alarm type.
    mcl_json_t *details;                    //!< Event/alarm details.
} event_payload_t;

/**
 * Handle for event.
 */
typedef struct mcl_event_t
{
    mcl_item_t item_base;     //!< Item base of event.
    event_payload_t *payload; //!< Payload of event.
} event_t;

/**
 * This function checks whether all mandatory parameters of an event are set or not.
 *
 * @param [in] event Event to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case event has one or more missing mandatory parameters.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t event_validate(event_t *event);

extern const char *mcl_event_versions[];

#endif //EVENT_H_
