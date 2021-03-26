/*!
 * @file     deployment_json.c
 * @brief    Implementation of deployment json operations.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "deployment_json.h"
#include "mcl_deployment/mcl_deployment_workflow.h"
#include "deployment_workflow.h"
#include "mcl_core/mcl_string_util.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_json_util.h"

// Function to parse json object into a deployment workflow.
static mcl_error_t _parse_workflow(mcl_json_t *json, deployment_workflow_t **workflow);

// Function to get the current state from workflow json object.
static mcl_error_t _get_current_state(mcl_json_t *json, deployment_workflow_state_t **current_state);

// Function to get a copy of the data json object from workflow json object.
static mcl_error_t _get_workflow_data(mcl_json_t *json, mcl_json_t **data);

// Function to parse json object into a deployment workflow state.
static mcl_error_t _parse_workflow_state(mcl_json_t* json, deployment_workflow_state_t **workflow_state);

// Function to parse json object into a deployment workflow history.
static mcl_error_t _parse_workflow_history(mcl_json_t *json, mcl_list_t **history);

// Function to parse a deployment workflow model from deployment workflow json object.
static mcl_error_t _parse_workflow_model(mcl_json_t *json, deployment_workflow_model_t **model);

// Function to parse states array of a deployment workflow model from json object of deployment workflow model.
static mcl_error_t _parse_workflow_model_states(mcl_json_t *json, mcl_list_t **states);

// Function to parse a model state from states array of a deployment workflow model.
static mcl_error_t _parse_workflow_model_state(mcl_json_t *json, deployment_workflow_model_state_t **state);

// Function to parse transitions of a deployment workflow model from json object of deployment workflow model.
static mcl_error_t _parse_workflow_model_transitions(mcl_json_t *json, mcl_list_t **transitions);

// Function to parse a model transition from transitions array of a deployment workflow model.
static mcl_error_t _parse_workflow_model_transition(mcl_json_t *json, deployment_workflow_model_transition_t **transition);

// Function to parse state groups of a deployment workflow model from json object of deployment workflow model.
static mcl_error_t _parse_workflow_model_state_groups(mcl_json_t *json, mcl_list_t **state_groups);

// Function to parse a model state group from state groups array of a deployment workflow model.
static mcl_error_t _parse_workflow_model_state_group(mcl_json_t *json, deployment_workflow_model_state_group_t **state_group);

// Function to get string value of a field from json object.
static mcl_error_t _parse_string_value(mcl_json_t *json, const char *field, char **value);

// Function to get float value of a field from json object.
static mcl_error_t _parse_float_value(mcl_json_t *json, const char *field, double *value);

// Function to get bool value of a field from json object.
static mcl_error_t _parse_bool_value(mcl_json_t *json, const char *field, mcl_bool_t *value);

mcl_error_t deployment_json_parse_workflows(const char *json_string, mcl_size_t string_length, mcl_list_t **workflows)
{
    mcl_error_t code;
    mcl_json_t *json = MCL_NULL;
    mcl_json_t *workflows_json_array;

    MCL_DEBUG_ENTRY("const char *json_string = <%p>, mcl_size_t string_length = <%lu>, mcl_list_t **workflows = <%p>",
        json_string, (long unsigned) string_length, workflows);

    code = mcl_json_util_parse(json_string, string_length, &json);

    if (MCL_OK == code)
    {
        code = mcl_json_util_get_object_item(json, "content", &workflows_json_array);
    }

    if (MCL_OK == code)
    {
        mcl_size_t index;

        // Get array size.
        mcl_size_t array_size;
        code = mcl_json_util_get_array_size(workflows_json_array, &array_size);

        if (MCL_OK == code)
        {
            code = mcl_list_initialize(workflows);
        }

        for (index = 0; (MCL_OK == code) && (index < array_size); ++index)
        {
            mcl_json_t *item;
            deployment_workflow_t *workflow = MCL_NULL;

            // Get item from array.
            code = mcl_json_util_get_array_item(workflows_json_array, (int) index, &item);

            // Parse the workflow item.
            if (MCL_OK == code)
            {
                code = _parse_workflow(item, &workflow);
            }

            // Add parsed struct to the list.
            if (MCL_OK == code)
            {
                code = mcl_list_add(*workflows, workflow);
            }

            // Clean up in case of an error at any stage of the loop.
            if (MCL_OK != code)
            {
                mcl_deployment_workflow_destroy(&workflow);
                mcl_list_destroy_with_content(workflows, (mcl_list_item_destroy_callback) mcl_deployment_workflow_destroy);
                break;
            }
        }
    }

    mcl_json_util_destroy(&json);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t deployment_json_parse_workflow(const char *json_string, mcl_size_t string_length, deployment_workflow_t **workflow)
{
    mcl_error_t code;
    mcl_json_t *json = MCL_NULL;

    MCL_DEBUG_ENTRY("const char *json_string = <%p>, mcl_size_t string_length = <%lu>, deployment_workflow_t **workflow = <%p>",
        json_string, (long unsigned) string_length, workflow);

    code = mcl_json_util_parse(json_string, string_length, &json);

    // Parse the workflow item.
    if (MCL_OK == code)
    {
        code = _parse_workflow(json, workflow);
    }

    mcl_json_util_destroy(&json);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t deployment_json_from_workflow_state(deployment_workflow_state_t *workflow_state, char **json_string)
{
    mcl_error_t code;
    mcl_json_t *json_object = MCL_NULL;

    // Make sure json string is null.
    *json_string = MCL_NULL;

    MCL_DEBUG_ENTRY("deployment_workflow_state_t *workflow_state = <%p>, char **json_string = <%p>", workflow_state, json_string);

    // Initialize a json object.
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &json_object);

    // Add state.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(json_object, "state", workflow_state->state);
    }

    // Add progress.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_double(json_object, "progress", workflow_state->progress);
    }

    // Add message.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(json_object, "message", workflow_state->message);
    }

    // Add details.
    if (MCL_OK == code)
    {
        mcl_json_t *details_json = MCL_NULL;

        if (MCL_NULL == workflow_state->details)
        {
            code = mcl_json_util_initialize(MCL_JSON_OBJECT, &details_json);
        }
        else
        {
            code = mcl_json_util_duplicate(workflow_state->details, &details_json);
        }

        if (MCL_OK == code)
        {
            code = mcl_json_util_add_object(json_object, "details", details_json);
        }

        if (MCL_OK != code)
        {
            mcl_json_util_destroy(&details_json);
        }
    }

    // Get json string from json object.
    if (MCL_OK == code)
    {
        code = mcl_json_util_to_string(json_object, json_string);
    }

    mcl_json_util_destroy(&json_object);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_workflow(mcl_json_t *json, deployment_workflow_t **workflow)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, deployment_workflow_t **workflow = <%p>", json, workflow);

    code = deployment_workflow_initialize(workflow);

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "id", &((*workflow)->id));
    }

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "deviceId", &((*workflow)->device_id));
    }

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "createdAt", &((*workflow)->created_at));
    }

    if (MCL_OK == code)
    {
        code = _get_current_state(json, &((*workflow)->current_state));
    }

    if (MCL_OK == code)
    {
        code = _parse_workflow_history(json, &((*workflow)->history));

        // History is an optional field.
        if (MCL_JSON_NON_EXISTING_CHILD == code)
        {
            code = MCL_OK;
        }
    }

    if (MCL_OK == code)
    {
        code = _parse_workflow_model(json, &((*workflow)->model));

        // Model is an optional field.
        if (MCL_JSON_NON_EXISTING_CHILD == code)
        {
            code = MCL_OK;
        }
    }

    // Get data field as json object since it is custom.
    if (MCL_OK == code)
    {
        code = _get_workflow_data(json, &((*workflow)->data));
    }

    if (MCL_OK != code)
    {
        mcl_deployment_workflow_destroy(workflow);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _get_current_state(mcl_json_t *json, deployment_workflow_state_t **current_state)
{
    mcl_error_t code;
    mcl_json_t *object;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, deployment_workflow_state_t **current_state = <%p>", json, current_state);

    code = mcl_json_util_get_object_item(json, "currentState", &object);

    if (MCL_OK == code)
    {
        code = _parse_workflow_state(object, current_state);
    }
    else
    {
        MCL_ERROR_STRING("currentState field is missing in workflow instance description.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_workflow_state(mcl_json_t *json, deployment_workflow_state_t **workflow_state)
{
    mcl_error_t code;
    mcl_json_t *details;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, deployment_workflow_state_t **workflow_state = <%p>", json, workflow_state);

    code = deployment_workflow_state_initialize(workflow_state);

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "state", &((*workflow_state)->state));
    }

    if (MCL_OK == code)
    {
        code = _parse_float_value(json, "progress", &((*workflow_state)->progress));
    }

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "entered", &((*workflow_state)->entered));
    }

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "updated", &((*workflow_state)->updated));
    }

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "message", &((*workflow_state)->message));
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_get_object_item(json, "details", &details);
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_duplicate(details, &((*workflow_state)->details));
    }

    if (MCL_OK != code)
    {
        mcl_deployment_workflow_state_destroy(workflow_state);
        MCL_ERROR_STRING("Failed to parse json object into mcl_deployment_workflow_state_t object.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_workflow_history(mcl_json_t *json, mcl_list_t **history)
{
    mcl_error_t code;
    mcl_json_t *json_array;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, mcl_list_t **history = <%p>", json, history);

    // Make sure history is null initially.
    *history = MCL_NULL;

    // Get history field of json object.
    code = mcl_json_util_get_object_item(json, "history", &json_array);

    // Initialize the list for history.
    if (MCL_OK == code)
    {
        code = mcl_list_initialize(history);
    }

    // Get each entry of workflow states in history into the list.
    if (MCL_OK == code)
    {
        mcl_size_t index;

        // Get number of entries in history.
        mcl_size_t array_size;
        code = mcl_json_util_get_array_size(json_array, &array_size);

        for (index = 0; (MCL_OK == code) && (index < array_size); ++index)
        {
            mcl_json_t *item;
            deployment_workflow_state_t *workflow_state = MCL_NULL;

            // Get item from array.
            code = mcl_json_util_get_array_item(json_array, (int) index, &item);

            // Parse the workflow state item.
            if (MCL_OK == code)
            {
                code = _parse_workflow_state(item, &workflow_state);
            }

            // Add parsed struct to the list.
            if (MCL_OK == code)
            {
                code = mcl_list_add(*history, workflow_state);
            }

            if (MCL_OK != code)
            {
                mcl_deployment_workflow_state_destroy(&workflow_state);
            }
        }
    }

    // In case of an error in parsing, make sure the list is destroyed with its content.
    if (MCL_OK != code)
    {
        mcl_list_destroy_with_content(history, (mcl_list_item_destroy_callback) mcl_deployment_workflow_state_destroy);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_workflow_model(mcl_json_t *json, deployment_workflow_model_t **model)
{
    mcl_error_t code;
    mcl_json_t *object;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, deployment_workflow_model_t **model = <%p>", json, model);

    *model = MCL_NULL;

    // Get model field of json object.
    code = mcl_json_util_get_object_item(json, "model", &object);

    if (MCL_OK == code)
    {
        code = deployment_workflow_model_initialize(model);
    }

    if (MCL_OK == code)
    {
        code = _parse_string_value(object, "key", &((*model)->key));
    }

    if (MCL_OK == code)
    {
        code = _parse_workflow_model_states(object, &((*model)->states));
    }

    if (MCL_OK == code)
    {
        code = _parse_workflow_model_transitions(object, &((*model)->transitions));
    }

    if (MCL_OK == code)
    {
        code = _parse_workflow_model_state_groups(object, &((*model)->state_groups));
    }

    // Return MCL_OK even if any child does not exist in json object.
    if (MCL_JSON_NON_EXISTING_CHILD == code)
    {
        code = MCL_OK;
    }

    if (MCL_OK != code)
    {
        deployment_workflow_model_destroy(model);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_workflow_model_states(mcl_json_t *json, mcl_list_t **states)
{
    mcl_error_t code;
    mcl_json_t *json_array;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, mcl_list_t **states = <%p>", json, states);

    *states = MCL_NULL;

    // Get array from json object.
    code = mcl_json_util_get_object_item(json, "states", &json_array);

    // Initialize the list.
    if (MCL_OK == code)
    {
        code = mcl_list_initialize(states);
    }

    if (MCL_OK == code)
    {
        mcl_size_t index;
        mcl_size_t array_size;

        // Get array size.
        code = mcl_json_util_get_array_size(json_array, &array_size);

        // Loop over each item in the array.
        for (index = 0; (MCL_OK == code) && (index < array_size); ++index)
        {
            mcl_json_t *item;
            deployment_workflow_model_state_t *state = MCL_NULL;

            // Get json item from array.
            code = mcl_json_util_get_array_item(json_array, (int) index, &item);

            // Parse array item.
            if (MCL_OK == code)
            {
                code = _parse_workflow_model_state(item, &state);
            }

            // Add parsed struct to the list.
            if (MCL_OK == code)
            {
                code = mcl_list_add(*states, state);
            }

            if (MCL_OK != code)
            {
                deployment_workflow_model_state_destroy(&state);
            }
        }
    }

    // In case of an error in parsing, make sure the list is destroyed with its content.
    if (MCL_OK != code)
    {
        mcl_list_destroy_with_content(states, (mcl_list_item_destroy_callback) deployment_workflow_model_state_destroy);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_workflow_model_state(mcl_json_t *json, deployment_workflow_model_state_t **state)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, deployment_workflow_model_state_t **state = <%p>", json, state);

    code = deployment_workflow_model_state_initialize(state);

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "name", &((*state)->name));
    }

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "description", &((*state)->description));
    }

    if (MCL_OK == code)
    {
        code = _parse_bool_value(json, "initial", &((*state)->initial));
    }

    if (MCL_OK == code)
    {
        code = _parse_bool_value(json, "final", &((*state)->final));
    }

    if (MCL_OK == code)
    {
        code = _parse_bool_value(json, "cancel", &((*state)->cancel));
    }

    if (MCL_NON_EXISTING_JSON_CHILD == code)
    {
        code = MCL_OK;
    }

    if (MCL_OK != code)
    {
        deployment_workflow_model_state_destroy(state);
        MCL_ERROR_STRING("Failed to parse json object into deployment_workflow_model_state_t object.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_workflow_model_transitions(mcl_json_t *json, mcl_list_t **transitions)
{
    mcl_error_t code;
    mcl_json_t *json_array;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, mcl_list_t **transitions = <%p>", json, transitions);

    *transitions = MCL_NULL;

    // Get array from json object.
    code = mcl_json_util_get_object_item(json, "transitions", &json_array);

    // Initialize the list.
    if (MCL_OK == code)
    {
        code = mcl_list_initialize(transitions);
    }

    if (MCL_OK == code)
    {
        mcl_size_t index;
        mcl_size_t array_size;

        // Get array size.
        code = mcl_json_util_get_array_size(json_array, &array_size);

        // Loop over each item in the array.
        for (index = 0; (MCL_OK == code) && (index < array_size); ++index)
        {
            mcl_json_t *item;
            deployment_workflow_model_transition_t *transition = MCL_NULL;

            // Get json item from array.
            code = mcl_json_util_get_array_item(json_array, (int) index, &item);

            // Parse array item.
            if (MCL_OK == code)
            {
                code = _parse_workflow_model_transition(item, &transition);
            }

            // Add parsed struct to the list.
            if (MCL_OK == code)
            {
                code = mcl_list_add(*transitions, transition);
            }

            if (MCL_OK != code)
            {
                deployment_workflow_model_transition_destroy(&transition);
            }
        }
    }

    // In case of an error in parsing, make sure the list is destroyed with its content.
    if (MCL_OK != code)
    {
        mcl_list_destroy_with_content(transitions, (mcl_list_item_destroy_callback) deployment_workflow_model_transition_destroy);
        MCL_ERROR_STRING("Failed to get transitions of deployment workflow model.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_workflow_model_transition(mcl_json_t *json, deployment_workflow_model_transition_t **transition)
{
    mcl_error_t code;
    mcl_json_t *details;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, deployment_workflow_model_transition_t **transition = <%p>", json, transition);

    code = deployment_workflow_model_transition_initialize(transition);

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "from", &((*transition)->from));
    }

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "to", &((*transition)->to));
    }

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "type", &((*transition)->type));
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_get_object_item(json, "details", &details);
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_duplicate(details, &((*transition)->details));
    }

    if (MCL_NON_EXISTING_JSON_CHILD == code)
    {
        code = MCL_OK;
    }

    if (MCL_OK != code)
    {
        deployment_workflow_model_transition_destroy(transition);
        MCL_ERROR_STRING("Failed to parse json object into deployment_workflow_model_transition_t object.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_workflow_model_state_groups(mcl_json_t *json, mcl_list_t **state_groups)
{
    mcl_error_t code;
    mcl_json_t *json_array;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, mcl_list_t **state_groups = <%p>", json, state_groups);

    *state_groups = MCL_NULL;

    // Get array from json object.
    code = mcl_json_util_get_object_item(json, "stateGroups", &json_array);

    // Initialize the list.
    if (MCL_OK == code)
    {
        code = mcl_list_initialize(state_groups);
    }

    if (MCL_OK == code)
    {
        mcl_size_t index;
        mcl_size_t array_size;

        // Get array size.
        code = mcl_json_util_get_array_size(json_array, &array_size);

        // Loop over each item in the array.
        for (index = 0; (MCL_OK == code) && (index < array_size); ++index)
        {
            mcl_json_t *item;
            deployment_workflow_model_state_group_t *state_group = MCL_NULL;

            // Get json item from array.
            code = mcl_json_util_get_array_item(json_array, (int)index, &item);

            // Parse array item.
            if (MCL_OK == code)
            {
                code = _parse_workflow_model_state_group(item, &state_group);
            }

            // Add parsed struct to the list.
            if (MCL_OK == code)
            {
                code = mcl_list_add(*state_groups, state_group);
            }

            if (MCL_OK != code)
            {
                deployment_workflow_model_state_destroy(&state_group);
            }
        }
    }

    // In case of an error in parsing, make sure the list is destroyed with its content.
    if (MCL_OK != code)
    {
        mcl_list_destroy_with_content(state_groups, (mcl_list_item_destroy_callback) deployment_workflow_model_state_group_destroy);
        MCL_ERROR_STRING("Failed to get state groups of deployment workflow model.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_workflow_model_state_group(mcl_json_t *json, deployment_workflow_model_state_group_t **state_group)
{
    mcl_error_t code;
    mcl_json_t *json_array;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, deployment_workflow_model_state_group_t **state_group = <%p>", json, state_group);

    code = deployment_workflow_model_state_group_initialize(state_group);

    if (MCL_OK == code)
    {
        code = _parse_string_value(json, "name", &((*state_group)->name));
    }

    // Get array from json object.
    if (MCL_OK == code)
    {
        code = mcl_json_util_get_object_item(json, "states", &json_array);
    }

    // Initialize the list.
    if (MCL_OK == code)
    {
        code = mcl_list_initialize(&((*state_group)->states));
    }

    if (MCL_OK == code)
    {
        mcl_size_t index;
        mcl_size_t array_size;

        // Get array size.
        code = mcl_json_util_get_array_size(json_array, &array_size);

        // Loop over each item in the array.
        for (index = 0; (MCL_OK == code) && (index < array_size); ++index)
        {
            mcl_json_t *item;
            char *state_name = MCL_NULL;

            // Get json item from array.
            code = mcl_json_util_get_array_item(json_array, (int) index, &item);

            // Parse array item.
            if (MCL_OK == code)
            {
                code = mcl_json_util_get_string(item, &state_name);
            }

            // Add parsed struct to the list.
            if (MCL_OK == code)
            {
                code = mcl_list_add((*state_group)->states, state_name);
            }

            if (MCL_OK != code)
            {
                MCL_FREE(state_name);
            }
        }
    }

    if (MCL_NON_EXISTING_JSON_CHILD == code)
    {
        code = MCL_OK;
    }

    if (MCL_OK != code)
    {
        deployment_workflow_model_state_group_destroy(state_group);
        MCL_ERROR_STRING("Failed to parse json object into deployment_workflow_model_state_group_t object.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _get_workflow_data(mcl_json_t *json, mcl_json_t **data)
{
    mcl_error_t code;
    mcl_json_t *object;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, mcl_json_t *json = <%p>", json, data);

    code = mcl_json_util_get_object_item(json, "data", &object);

    if (MCL_OK == code)
    {
        code = mcl_json_util_duplicate(object, data);
    }
    else
    {
        MCL_ERROR_STRING("data field is missing in workflow instance description.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_string_value(mcl_json_t *json, const char *field, char **value)
{
    mcl_error_t code;
    mcl_json_t *object;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, const char *field = <%p>, char **value = <%p>", json, field, value);

    code = mcl_json_util_get_object_item(json, field, &object);

    if (MCL_OK == code)
    {
        code = mcl_json_util_get_string(object, value);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_float_value(mcl_json_t *json, const char *field, double *value)
{
    mcl_error_t code;
    mcl_json_t *object;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, const char *field = <%p>, double *value = <%p>", json, field, value);

    code = mcl_json_util_get_object_item(json, field, &object);

    if (MCL_OK == code)
    {
        code = mcl_json_util_get_double_value(object, value);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_bool_value(mcl_json_t *json, const char *field, mcl_bool_t *value)
{
    mcl_error_t code;
    mcl_json_t *object;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, const char *field = <%p>, mcl_bool_t **value = <%p>", json, field, value);

    code = mcl_json_util_get_object_item(json, field, &object);

    if (MCL_OK == code)
    {
        code = mcl_json_util_get_bool_value(object, value);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
