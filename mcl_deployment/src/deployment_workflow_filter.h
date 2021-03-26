/*!
 * @file     deployment_workflow_filter.h
 * @brief    Internal header for deployment workflow filter.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DEPLOYMENT_WORKFLOW_FILTER_H_
#define DEPLOYMENT_WORKFLOW_FILTER_H_

#include "mcl_deployment/mcl_deployment_common.h"

/**
 * Handle for deployment workflow filter.
 */
typedef struct mcl_deployment_workflow_filter_t
{
    mcl_bool_t model;
    mcl_bool_t history;
    char *current_state;
    char *group;
    char *device_id;
    char *model_key;
} deployment_workflow_filter_t;

#endif //DEPLOYMENT_WORKFLOW_FILTER_H_
