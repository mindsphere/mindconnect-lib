/*!
 * @file     random.c
 * @brief    Random module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "random.h"
#include "security.h"
#include "string_util.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_random.h"
#include "mcl_core/mcl_memory.h"

#if defined(HAVE_STDLIB_H_) && (1 == HAVE_STDLIB_H_)
#include <stdlib.h>
#endif

#define GUID_LENGTH 36

typedef union guid_t
{
    struct
    {
        mcl_uint32_t time_low;            //!< Lowest significant 32 bits of time.
        mcl_uint16_t time_mid;            //!< Middle 16 bits of time.
        mcl_uint16_t time_hi_and_version; //!< Version followed by 12 highest significant bits of time.
        mcl_uint8_t clk_seq_hi_res;       //!< Variant followed by highest bits of clock sequence.
        mcl_uint8_t clk_seq_low;          //!< Lowest significant bits of clock sequence.
        mcl_uint8_t node[6];              //!< Node ID.
    } guid_fields;

    mcl_uint8_t rnd[16];                  //!< Random bytes.
} guid_t;

mcl_error_t mcl_random_generate_guid(char **guid)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("char **guid = <%p>", guid);

    MCL_ASSERT_NOT_NULL(guid, code);

    code = random_generate_guid(guid);
    MCL_ASSERT_CODE_MESSAGE(MCL_OK == code, code, "GUID can not be generated.");

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_random_generate_bytes(unsigned char *buffer, mcl_size_t size)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("unsigned char *buffer = <%u>, mcl_size_t size = <%u>", buffer, size);

    MCL_ASSERT_NOT_NULL(buffer, code);

    code = security_generate_random_bytes(buffer, size);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t random_generate_number(mcl_uint32_t *random_number)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_uint32_t *random_number = <%p>", random_number);

    code = security_generate_random_bytes((unsigned char *) random_number, sizeof(mcl_uint32_t));

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t random_generate_guid(char **guid)
{
    mcl_error_t code;
    guid_t guid_content;
    char *guid_string;

    MCL_DEBUG_ENTRY("char **guid = <%p>", guid);

    guid_string = MCL_MALLOC(GUID_LENGTH + 1);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != guid_string, MCL_OUT_OF_MEMORY, "Memory can not be allocated for GUID string.");

    code = security_generate_random_bytes(guid_content.rnd, sizeof(guid_content));

    if (MCL_OK == code)
    {
        guid_content.guid_fields.clk_seq_hi_res = (guid_content.guid_fields.clk_seq_hi_res & 0x3FU) | 0x80U;
        guid_content.guid_fields.time_hi_and_version = (guid_content.guid_fields.time_hi_and_version & 0x0FFFU) | 0x4000U;

        code = string_util_snprintf(guid_string, GUID_LENGTH + 1, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", guid_content.guid_fields.time_low, guid_content.guid_fields.time_mid,
                                    guid_content.guid_fields.time_hi_and_version, guid_content.guid_fields.clk_seq_hi_res, guid_content.guid_fields.clk_seq_low, guid_content.guid_fields.node[0], guid_content.guid_fields.node[1],
                                    guid_content.guid_fields.node[2], guid_content.guid_fields.node[3], guid_content.guid_fields.node[4], guid_content.guid_fields.node[5]);
        MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, MCL_FREE(guid_string), code, "GUID structure can not be printed as string.");

        *guid = guid_string;
    }
    else
    {
        MCL_FREE(guid_string);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
