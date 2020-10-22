/*!
 * @file     base64.c
 * @brief    Base64 module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "base64.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

// Define padding char.
#define PADDING_CHAR '='

// Define size of a quantum (4 bytes data group).
#define QUANTUM_SIZE 4
#define INPUT_GROUP_SIZE 3

// Only maximum of 2 padding chars are allowed.
#define MAX_PADDING_CHAR_COUNT 2
#define BASE64_BIT_COUNT_PER_CHARACTER  6

// Mask for last 6 bit, 00111111
#define MASK_FOR_ENCODING   0x3FU

// Mask for least significant byte.
#define MASK_FOR_LOW_BYTE 0xFFU
#define BYTE_BIT_COUNT 8

// Base64 encoding table.
static const char base64_encode_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//J-
// Base64 decoding table.
static const mcl_uint8_t base64_decode_table[] =
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
//J+

// Base64 url encoding table (see RFC 4648 document section 5).
static const char base64_url_encode_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

//J-
// Base64 url decoding table.
static const mcl_uint8_t base64_url_decode_table[] =
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F,
    0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
//J+

// This function is used to encode data by using table.
static mcl_error_t _encode_with_table(const char *table, const mcl_uint8_t *data, mcl_size_t data_size, char **encoded_data);

// This function is used to decode data by using table.
static mcl_error_t _decode_with_table(const mcl_uint8_t *table, const char *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size);

// This function is used to decode quantum.
static mcl_error_t _decode_quantum(const mcl_uint8_t *table, const char *source, mcl_uint8_t *destination);

mcl_error_t base64_decode(const char *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("const char *encoded_data = <%p>, mcl_uint8_t **decoded_data = <%p>, mcl_size_t *decoded_data_size = <%p>", encoded_data, decoded_data, decoded_data_size);

    code = _decode_with_table(base64_decode_table, encoded_data, decoded_data, decoded_data_size);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t base64_url_decode(const char *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("const char *encoded_data = <%p>, mcl_uint8_t **decoded_data = <%p>, mcl_size_t *decoded_data_size = <%p>", encoded_data, decoded_data, decoded_data_size);

    code = _decode_with_table(base64_url_decode_table, encoded_data, decoded_data, decoded_data_size);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t base64_encode(const mcl_uint8_t *data, mcl_size_t data_size, char **encoded_data)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, char **encoded_data = <%p>", data, data_size, encoded_data);

    code = _encode_with_table(base64_encode_table, data, data_size, encoded_data);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t base64_url_encode(const mcl_uint8_t *data, mcl_size_t data_size, char **encoded_data)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, char **encoded_data = <%p>", data, data_size, encoded_data);

    code = _encode_with_table(base64_url_encode_table, data, data_size, encoded_data);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _decode_with_table(const mcl_uint8_t *table, const char *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t number_of_quantums;
    mcl_size_t index = 0;
    mcl_uint8_t padding_count = 0;
    mcl_size_t encoded_data_length;
    const char *encoded_position = encoded_data;

    MCL_DEBUG_ENTRY("const mcl_uint8_t *table = <%p>, const char *encoded_data = <%p>, mcl_uint8_t **decoded_data = <%p>, mcl_size_t *decoded_data_size = <%p>", table, encoded_data, decoded_data, decoded_data_size);

    *decoded_data = MCL_NULL;
    *decoded_data_size = 0;

    encoded_data_length = mcl_string_util_strlen(encoded_data);

    // Check the length of the input string is valid.
    MCL_ASSERT_CODE_MESSAGE((encoded_data_length > 0) && (encoded_data_length % QUANTUM_SIZE == 0), MCL_BAD_CONTENT_ENCODING,
                        "Length of encoded data must be positive and multiples of 4.");

    // Look for padding characters.
    for (index = (encoded_data_length - 1); (index > 0) && (MCL_OK == code); --index)
    {
        if (PADDING_CHAR == encoded_data[index])
        {
            ++padding_count;

            if (MAX_PADDING_CHAR_COUNT < padding_count)
            {
                code = MCL_BAD_CONTENT_ENCODING;
            }
        }
        else
        {
            break;
        }
    }

    // Verify that there is no padding character until the end.
    if (MCL_OK == code)
    {
        for (index = 0; index < (encoded_data_length - (1 + padding_count)); ++index)
        {
            if (PADDING_CHAR == encoded_data[index])
            {
                code = MCL_BAD_CONTENT_ENCODING;
                break;
            }
        }
    }

    if (MCL_OK == code)
    {
        // Calculate the number of quantums.
        number_of_quantums = encoded_data_length / QUANTUM_SIZE;
        MCL_VERBOSE("Number of quantums (4 Byte groups) = <%lu>", number_of_quantums);

        // Calculate the size of the decoded string.
        *decoded_data_size = (number_of_quantums * INPUT_GROUP_SIZE) - padding_count;
        MCL_VERBOSE("Length of resulting string will be = <%lu>", *decoded_data_size);

        // Allocate buffer.
        (*decoded_data) = MCL_MALLOC((*decoded_data_size) + MCL_NULL_CHAR_SIZE);

        if (MCL_NULL == (*decoded_data))
        {
            code = MCL_OUT_OF_MEMORY;
        }
    }

    if (MCL_OK == code)
    {
        mcl_uint8_t *decode_position;

        decode_position = *decoded_data;

        // Decode the quantums.
        for (index = 0; (index < number_of_quantums) && (MCL_OK == code); ++index)
        {
            code = _decode_quantum(table, encoded_position, decode_position);

            decode_position += INPUT_GROUP_SIZE;
            encoded_position += QUANTUM_SIZE;
        }

        (*decoded_data)[*decoded_data_size] = MCL_NULL_CHAR;
    }

    if (MCL_OK != code)
    {
        MCL_FREE(*decoded_data);
        *decoded_data_size = 0;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _decode_quantum(const mcl_uint8_t *table, const char *source, mcl_uint8_t *destination)
{
    mcl_error_t code = MCL_OK;
    mcl_int8_t index;
    mcl_uint32_t data = 0;
    mcl_uint8_t padding_count = 0;

    MCL_DEBUG_ENTRY("const mcl_uint8_t *table = <%p>, const char *source = <%s>, mcl_uint8_t *destination = <%p>", table, source, destination);

    // Compose 24 bit value using encoded characters.
    for (index = 0; (index < QUANTUM_SIZE) && (MCL_OK == code); ++index)
    {
        char current_char = source[index];

        if (PADDING_CHAR == current_char)
        {
            data = (data << BASE64_BIT_COUNT_PER_CHARACTER);
            ++padding_count;

            if (MAX_PADDING_CHAR_COUNT < padding_count)
            {
                code = MCL_BAD_CONTENT_ENCODING;
            }
        }
        else
        {
            mcl_uint8_t table_index = table[(mcl_uint8_t) current_char];
            mcl_uint8_t invalid_index = 0xFF;

            if (invalid_index == table_index)
            {
                code = MCL_BAD_CONTENT_ENCODING;
            }
            else
            {
                MCL_VERBOSE("Current char = <%c> in table found at index = <%u>", current_char, table_index);
                data = (data << BASE64_BIT_COUNT_PER_CHARACTER) + table_index;
            }
        }
    }

    // Decode 24 bit value.
    if (MCL_OK == code)
    {
        for (index = (INPUT_GROUP_SIZE - 1); index >= 0; --index)
        {
            if (INPUT_GROUP_SIZE > (padding_count + index))
            {
                destination[index] = (mcl_uint8_t)data & MASK_FOR_LOW_BYTE;
            }

            data = data >> BYTE_BIT_COUNT;
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _encode_with_table(const char *table, const mcl_uint8_t *data, mcl_size_t data_size, char **encoded_data)
{
    mcl_size_t iteration;
    mcl_size_t iteration_count;
    mcl_size_t buffer_size;
    mcl_uint8_t remainder;

    MCL_DEBUG_ENTRY("const char *table = <%p>, const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, char **encoded_data = <%p>", table, data, data_size, encoded_data);

    iteration_count = data_size / INPUT_GROUP_SIZE;
    remainder = data_size % INPUT_GROUP_SIZE;

    buffer_size = (iteration_count * QUANTUM_SIZE) + MCL_NULL_CHAR_SIZE;

    if (0 != remainder)
    {
        buffer_size += QUANTUM_SIZE;
    }

    // Return pointer to new data.
    *encoded_data = MCL_MALLOC(buffer_size);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != *encoded_data, MCL_OUT_OF_MEMORY, "Memory to store encoded data couldn't be allocated!");

    for (iteration = 0; iteration < iteration_count; ++iteration)
    {
        mcl_int8_t index;
        mcl_uint32_t input = (mcl_uint32_t) data[iteration * INPUT_GROUP_SIZE] << 16;

        input |= (mcl_uint32_t) data[(iteration * INPUT_GROUP_SIZE) + 1] << BYTE_BIT_COUNT;
        input |= (mcl_uint32_t) data[(iteration * INPUT_GROUP_SIZE) + 2];

        for (index = (QUANTUM_SIZE - 1); index >= 0; --index)
        {
            (*encoded_data)[(iteration * QUANTUM_SIZE) + index] = table[input & MASK_FOR_ENCODING];
            input = (input >> BASE64_BIT_COUNT_PER_CHARACTER);
        }
    }

    if (0 != remainder)
    {
        mcl_int8_t index;
        mcl_uint32_t input = (mcl_uint32_t) data[iteration_count * INPUT_GROUP_SIZE] << 16;

        if (1 < remainder)
        {
            input |= (mcl_uint32_t) data[(iteration * INPUT_GROUP_SIZE) + 1] << BYTE_BIT_COUNT;
        }

        for (index = (QUANTUM_SIZE - 1); index >= 0; --index)
        {
            if (index > remainder)
            {
                (*encoded_data)[(iteration_count * QUANTUM_SIZE) + index] = PADDING_CHAR;
            }
            else
            {
                (*encoded_data)[(iteration_count * QUANTUM_SIZE) + index] = table[input & MASK_FOR_ENCODING];
            }

            input = (input >> BASE64_BIT_COUNT_PER_CHARACTER);
        }
    }

    (*encoded_data)[buffer_size - 1] = MCL_NULL_CHAR;

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}
