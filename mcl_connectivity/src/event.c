/*!
 * @file     event.c
 * @brief    Event module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_connectivity/mcl_event.h"
#include "event.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_random.h"
#include "mcl_core/mcl_string_util.h"

#define MCL_EVENT_PARAMETER_DESCRIPTION_MAXIMUM_LENGTH 256

const char *mcl_event_versions[MCL_EVENT_VERSION_END] = { "1.0", "2.0" };

static mcl_int32_t _event_severity_values[MCL_EVENT_VERSION_END][MCL_EVENT_SEVERITY_END] =
{
    { 1, 2, 3 },
    { 20, 30, 40 }
};

mcl_error_t mcl_event_initialize(E_MCL_EVENT_VERSION version, mcl_event_t **event)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("E_MCL_EVENT_VERSION version = <%d>, mcl_event_t **event = <%p>", version, event);

    // Null check.
    MCL_ASSERT_NOT_NULL(event, code);

    // Check meta version parameter.
    MCL_ASSERT_CODE_MESSAGE(MCL_EVENT_VERSION_1_0 <= version && MCL_EVENT_VERSION_END > version, MCL_INVALID_PARAMETER,
        "Invalid meta payload version parameter.");

    // Allocate memory for event.
    if (MCL_NULL != MCL_NEW(*event))
    {
        // Set base parameters for event.
        (*event)->item_base.preamble = MCL_ITEM_PREAMBLE;
        (*event)->item_base.type = MCL_ITEM_TYPE_EVENT;
        (*event)->item_base.version = (mcl_uint32_t) version;

        // Allocate memory for event payload.
        MCL_NEW((*event)->payload);
    }

    if ((MCL_NULL != (*event)) && (MCL_NULL != (*event)->payload))
    {
        // Event payload.
        (*event)->payload->id = MCL_NULL;
        (*event)->payload->correlation_id = MCL_NULL;
        (*event)->payload->description = MCL_NULL;
        (*event)->payload->type = MCL_NULL;
        (*event)->payload->version = MCL_NULL;
        (*event)->payload->details = MCL_NULL;
        (*event)->payload->severity = _event_severity_values[version][MCL_EVENT_SEVERITY_INFORMATION];

        // Set payload id (Unique identifier of the event).
        code = mcl_random_generate_guid(&(*event)->payload->id);
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    // Error check.
    if (MCL_OK != code)
    {
        mcl_event_destroy(event);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_event_set_parameter(mcl_event_t *event, E_MCL_EVENT_PARAMETER parameter, const void *value)
{
    mcl_error_t code;
    mcl_size_t description_length;

    MCL_DEBUG_ENTRY("mcl_event_t *event = <%p>, E_MCL_EVENT_PARAMETER parameter = <%d>, const void *value = <%p>", event, parameter, value);

    // Null check.
    MCL_ASSERT_NOT_NULL(event, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_EVENT_PARAMETER_TYPE:
            code = mcl_string_util_reset(value, &event->payload->type);
            break;

        case MCL_EVENT_PARAMETER_VERSION:
            code = mcl_string_util_reset(value, &event->payload->version);
            break;

        case MCL_EVENT_PARAMETER_SEVERITY:

            // Validate payload severity parameter.
            MCL_ASSERT_CODE_MESSAGE(MCL_EVENT_SEVERITY_ERROR <= *(mcl_int32_t *) (value) &&
                MCL_EVENT_SEVERITY_END > *(mcl_int32_t *) (value), MCL_INVALID_PARAMETER, "Invalid severity value.");

            event->payload->severity = _event_severity_values[event->item_base.version][*((E_MCL_EVENT_SEVERITY*) value)];
            code = MCL_OK;
            break;

        case MCL_EVENT_PARAMETER_TIMESTAMP_ISO8601:

            // Validate payload timestamp parameter.
            if (MCL_TRUE == mcl_time_util_validate_timestamp(value))
            {
                mcl_string_util_memcpy(event->payload->timestamp, value, MCL_TIMESTAMP_LENGTH);
                code = MCL_OK;
            }
            else
            {
                code = MCL_INVALID_PARAMETER;
            }

            break;

        case MCL_EVENT_PARAMETER_TIMESTAMP_EPOCH:
            code = mcl_time_util_convert_to_iso_8601_format((const time_t *) value, event->payload->timestamp);
            break;

        case MCL_EVENT_PARAMETER_DESCRIPTION:

            // Check length of description value. Max 255 is allowed.
            description_length = mcl_string_util_strlen(value);
            MCL_ASSERT_CODE_MESSAGE((MCL_EVENT_PARAMETER_DESCRIPTION_MAXIMUM_LENGTH > description_length), MCL_INVALID_PARAMETER,
                "Length of description is not less than 256 characters." );

            code = mcl_string_util_reset(value, &event->payload->description);
            break;

        case MCL_EVENT_PARAMETER_CORRELATION_ID:
            code = mcl_string_util_reset(value, &event->payload->correlation_id);
            break;

        case MCL_EVENT_PARAMETER_DETAILS:
            mcl_json_util_destroy(&event->payload->details);
            code = mcl_json_util_duplicate((const mcl_json_t *) value, &event->payload->details);
            break;

        default:
            code = MCL_INVALID_PARAMETER;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t event_validate(event_t *event)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("event_t *event = <%p>", event);

    // Event validation checks.
    if (MCL_TRUE != mcl_time_util_validate_timestamp(event->payload->timestamp))
    {
        code = MCL_INVALID_PARAMETER;
    }
    else if (MCL_NULL == event->payload->type)
    {
        code = MCL_INVALID_PARAMETER;
    }
    else if (MCL_NULL == event->payload->version)
    {
        code = MCL_INVALID_PARAMETER;
    }
    else
    {
        code = MCL_OK;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_event_destroy(mcl_event_t **event)
{
    MCL_DEBUG_ENTRY("mcl_event_t **event = <%p>", event);

    // Destroys the event data structure.
    if ((MCL_NULL != event) && (MCL_NULL != *event))
    {
        if (MCL_NULL != (*event)->payload)
        {
            // Destroy payload.
            MCL_FREE((*event)->payload->id);
            MCL_FREE((*event)->payload->correlation_id);
            MCL_FREE((*event)->payload->description);
            MCL_FREE((*event)->payload->type);
            MCL_FREE((*event)->payload->version);
            mcl_json_util_destroy(&((*event)->payload->details));
            MCL_FREE((*event)->payload);
        }

        MCL_FREE(*event);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
