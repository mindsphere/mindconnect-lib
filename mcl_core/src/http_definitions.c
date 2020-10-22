/*!
 * @file     http_definitions.c
 * @brief    HTTP definitions module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "http_definitions.h"
#include "definitions.h"

const char *http_header_names[HTTP_HEADER_NAMES_END] =
{
    "Authorization",
    "Content-Type",
    "Content-ID",
    "Content-Length",
    "Transfer-Encoding",
    "Transfer-Encoding: chunked",
    "User-Agent",
    "Accept",
    "Host",
    "Content-Disposition",
    "Range",
    "Content-Range",
    "Server-Time",
    "If-Match",
    "ETag",
    "Correlation-ID"
};
