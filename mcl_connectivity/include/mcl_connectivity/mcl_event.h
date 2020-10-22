/*!
 * @file     mcl_event.h
 * @brief    Event module interface header file.
 *
 * Agents can upload event data to MindSphere. This module enables the agent to initialize an event
 * data type using #mcl_event_initialize function. Following initialization, the parameters of this
 * event data type can be set using #mcl_event_set_parameter function. Once the event data type is
 * ready, it can be uploaded to MindSphere using #mcl_connectivity_exchange function. Optionally, the
 * event can be added to #mcl_store_t and the store containing the event and possible other data can
 * be uploaded to MindSphere. If the event data type is exchanged as a single item, the agent is expected
 * to destroy the event after the exchange operation using #mcl_event_destroy function. If the event is
 * exchanged in a store, store will handle the destroy operation of the event and the agent does not need
 * to take any action.
 *
 * For more information, please look at example file "mcl_connectivity/examples/event_upload.c".
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_EVENT_H_
#define MCL_EVENT_H_

#include "mcl_connectivity/mcl_connectivity_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for event.
 */
typedef struct mcl_event_t mcl_event_t;

/**
 * Version of event item payload.
 */
typedef enum E_MCL_EVENT_VERSION
{
    MCL_EVENT_VERSION_1_0 = 0, //!< Event version 1.0.
    MCL_EVENT_VERSION_2_0,     //!< Event version 2.0.
    MCL_EVENT_VERSION_END      //!< End of event version enumeration.
} E_MCL_EVENT_VERSION;

/**
 * Severity values for events.
 */
typedef enum E_MCL_EVENT_SEVERITY
{
    MCL_EVENT_SEVERITY_ERROR = 0,   //!< Severity level of event is ERROR.
    MCL_EVENT_SEVERITY_WARNING,     //!< Severity level of event is WARNING.
    MCL_EVENT_SEVERITY_INFORMATION, //!< Severity level of event is INFORMATION.
    MCL_EVENT_SEVERITY_END
} E_MCL_EVENT_SEVERITY;

/**
 * Parameters for events.
 */
typedef enum E_MCL_EVENT_PARAMETER
{
    MCL_EVENT_PARAMETER_TYPE = 0,           //!< Event payload type parameter as char*.
    MCL_EVENT_PARAMETER_VERSION,            //!< Event payload version parameter as char*.
    MCL_EVENT_PARAMETER_SEVERITY,           //!< Event payload severity parameter as E_MCL_EVENT_SEVERITY.
    MCL_EVENT_PARAMETER_TIMESTAMP_ISO8601,  //!< Event payload timestamp parameter as char* (which is in yyyy-MM-ddTHH:mm:ss.SSSZ format).
    MCL_EVENT_PARAMETER_TIMESTAMP_EPOCH,    //!< Event payload timestamp parameter as time_t*.
    MCL_EVENT_PARAMETER_DESCRIPTION,        //!< Event payload description parameter as char* (optional).
    MCL_EVENT_PARAMETER_CORRELATION_ID,     //!< Event payload correlation id parameter as char* (optional).
    MCL_EVENT_PARAMETER_DETAILS,            //!< Event payload details parameter as mcl_json_t* (optional).
    MCL_EVENT_PARAMETER_END
} E_MCL_EVENT_PARAMETER;

/**
 * This function creates and initializes a data struct of #mcl_event_t.
 *
 * @param [in] version One of the versions listed in #E_MCL_EVENT_VERSION.
 * @param [out] event Event handle which is going to be initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER in case meta payload version parameter is invalid.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_event_initialize(E_MCL_EVENT_VERSION version, mcl_event_t **event);

/**
 * This function is used to set a parameter of an event.
 *
 * @param [in] event Event to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_EVENT_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p event or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_event_set_parameter(mcl_event_t *event, E_MCL_EVENT_PARAMETER parameter, const void *value);

/**
 * This function destroys event data structure.
 *
 * @param [in] event Event handle which is going to be destroyed.
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_event_destroy(mcl_event_t **event);

#ifdef  __cplusplus
}
#endif

#endif //MCL_EVENT_H_
