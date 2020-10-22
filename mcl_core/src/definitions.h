/*!
 * @file     definitions.h
 * @brief    Definitions module header file.
 *
 * General definitions are stored in this header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#define NONCE_SIZE 16

// TODO: version size assumed 36 instead of 255 like in document and DEFAULT_ID_SIZE is used. Change later after deciding.
// These sizes come from "Interface Specification MindConnect API V 0.1". Sizes increased by 1 for null termination.
#define DEFAULT_META_TYPE_SIZE                  6
#define DEFAULT_PAYLOAD_TYPE_SIZE               20
#define DEFAULT_QUALITY_CODE_SIZE               9
#define DEFAULT_DATE_SIZE                       23
#define DEFAULT_TIMESTAMP_SIZE                  25
#define DEFAULT_ID_SIZE                         37
#define DEFAULT_VALUES_COUNT                    5

#define MAXIMUM_HOST_NAME_LENGTH           256
#define MAXIMUM_PROXY_USER_NAME_LENGTH     32
#define MAXIMUM_PROXY_PASSWORD_LENGTH      32
#define MAXIMUM_PROXY_DOMAIN_LENGTH        256
#define MAXIMUM_USER_AGENT_LENGTH          512

// 300 seconds is default http request timeout value.
#define DEFAULT_HTTP_REQUEST_TIMEOUT (300)

// JWT used in authorization header has an expiration time of 24 hours.
#define JWT_EXPIRATION_TIME 86400

#endif //DEFINITIONS_H_
