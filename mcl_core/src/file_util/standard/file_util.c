/*!
 * @file     file_util.c
 * @brief    File utility module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "file_util.h"
#include "mcl_core/mcl_assert.h"
#include <stdio.h>

#if HAVE_FILE_SYSTEM_
#include <sys/stat.h>
#include <sys/types.h>

static mcl_error_t _file_util_fstat(void *file_descriptor, struct stat *file_attributes);
#else
static const char mcl_no_file_system_message[] = "MCL file utility cannot be used.";
#endif

#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

mcl_error_t mcl_file_util_fopen(const char *file_name, const char *mode, void **file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    mcl_error_t return_code;

    MCL_DEBUG_ENTRY("const char *file_name = <%s>, const char *mode = <%s>, void **file_descriptor = <%p>", file_name, mode, file_descriptor);

    return_code = mcl_file_util_fopen_without_log(file_name, mode, file_descriptor);

    MCL_DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
#else
    MCL_ERROR_STRING(mcl_no_file_system_message);
    return MCL_FAIL;
#endif
}

mcl_error_t mcl_file_util_fopen_without_log(const char *file_name, const char *mode, void **file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    mcl_error_t return_code = MCL_OK;
    FILE *file;

#if defined(WIN32) || defined(WIN64)
    fopen_s(&file, file_name, mode);
#else
    file = fopen(file_name, mode);
#endif
    *file_descriptor = file;
    if (MCL_NULL == file)
    {
        return_code = MCL_FAIL;
    }

    return return_code;
#else
    return MCL_FAIL;
#endif
}

mcl_error_t mcl_file_util_fclose(void *file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    mcl_error_t return_code;

    MCL_DEBUG_ENTRY("void *file_descriptor = <%p>", file_descriptor);

    return_code = mcl_file_util_fclose_without_log(file_descriptor);

    MCL_DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
#else
    MCL_ERROR_STRING(mcl_no_file_system_message);
    return MCL_FAIL;
#endif
}

mcl_error_t mcl_file_util_fclose_without_log(void *file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    mcl_error_t return_code = MCL_OK;

    if (MCL_NULL != file_descriptor)
    {
        mcl_error_t result = fclose((FILE *)file_descriptor);
        if (0 != result)
        {
            return_code = MCL_FAIL;
        }
    }

    return return_code;
#else
    return MCL_FAIL;
#endif
}

mcl_error_t mcl_file_util_fwrite(const void *data, mcl_size_t size, mcl_size_t count, void *file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    mcl_error_t return_code;

    MCL_DEBUG_ENTRY("const void *data = <%p>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *file_descriptor = <%p>",
        data, size, count, file_descriptor);

    return_code = mcl_file_util_fwrite_without_log(data, size, count, file_descriptor);

    MCL_DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
#else
    MCL_ERROR_STRING(mcl_no_file_system_message);
    return MCL_FAIL;
#endif
}

mcl_error_t mcl_file_util_fwrite_without_log(const void *data, mcl_size_t size, mcl_size_t count, void *file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    mcl_error_t return_code = MCL_FAIL;

    mcl_size_t actual_count = fwrite(data, size, count, (FILE *)file_descriptor);
    if (count == actual_count)
    {
        return_code = MCL_OK;
    }
    else
    {
        MCL_ERROR("Data can not be written to file.");
    }

    return return_code;
#else
    return MCL_FAIL;
#endif
}

void mcl_file_util_fread(void *data, mcl_size_t size, mcl_size_t count, void *file_descriptor, mcl_size_t *actual_count)
{
#if HAVE_FILE_SYSTEM_
    MCL_DEBUG_ENTRY("void *data = <%p>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *file_descriptor = <%p>, mcl_size_t *actual_count = <%p>",
        data, size, count, file_descriptor, actual_count);

    *actual_count = fread(data, size, count, (FILE *)file_descriptor);

    MCL_DEBUG_LEAVE("retVal = void");
#else
    MCL_ERROR_STRING(mcl_no_file_system_message);
#endif
}

mcl_error_t mcl_file_util_fputs(const char *data, void *file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    mcl_error_t return_code = MCL_FAIL;
    mcl_error_t result;

    MCL_DEBUG_ENTRY("const char *data = <%s>, void *file_descriptor = <%p>", data, file_descriptor);

    result = fputs(data, (FILE *)file_descriptor);
    if (0 <= result)
    {
        return_code = MCL_OK;
    }
    else
    {
        MCL_ERROR("Data can not be written to file.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
#else
    MCL_ERROR_STRING(mcl_no_file_system_message);
    return MCL_FAIL;
#endif
}

mcl_error_t mcl_file_util_fgets(char *data, mcl_size_t data_size, void *file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    mcl_error_t return_code = MCL_FAIL;
    char *output;

    MCL_DEBUG_ENTRY("char *data = <%p>, mcl_size_t data_size = <%u>, void *file_descriptor = <%p>", data, data_size, file_descriptor);

    output = fgets(data, (int)data_size, (FILE *)file_descriptor);
    if (MCL_NULL != output)
    {
        return_code = MCL_OK;
    }
    else
    {
        MCL_DEBUG("No data is read from file.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
#else
    MCL_ERROR_STRING(mcl_no_file_system_message);
    return MCL_FAIL;
#endif
}

mcl_error_t mcl_file_util_fflush(void *file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    mcl_error_t return_code;

    MCL_DEBUG_ENTRY("void *file_descriptor = <%p>", file_descriptor);

    return_code = mcl_file_util_fflush_without_log(file_descriptor);

    MCL_DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
#else
    MCL_ERROR_STRING(mcl_no_file_system_message);
    return MCL_FAIL;
#endif
}

mcl_error_t mcl_file_util_fflush_without_log(void *file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    mcl_error_t return_code = MCL_FAIL;
    int result;

    result = fflush((FILE *)file_descriptor);

    if (0 == result)
    {
        return_code = MCL_OK;
    }
    else
    {
        MCL_ERROR("Error in flush.");
    }

    return return_code;
#else
    MCL_ERROR_STRING(mcl_no_file_system_message);
    return MCL_FAIL;
#endif
}

void mcl_file_util_rewind(void *file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    MCL_DEBUG_ENTRY("void *file_descriptor = <%p>", file_descriptor);

    if (MCL_NULL != file_descriptor)
    {
        rewind((FILE *)file_descriptor);
    }

    MCL_DEBUG_LEAVE("retVal = void");
#else
    MCL_ERROR_STRING(mcl_no_file_system_message);
#endif
}

mcl_bool_t mcl_file_util_check_if_regular_file(void *file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    mcl_bool_t is_regular_file = MCL_FALSE;
    struct stat file_attributes;

    MCL_DEBUG_ENTRY("void *file_descriptor = <%p>", file_descriptor);

#ifdef S_ISREG
    if (MCL_OK == _file_util_fstat(file_descriptor, &file_attributes))
    {
        is_regular_file = S_ISREG(file_attributes.st_mode);
    }
#endif

    MCL_DEBUG_LEAVE("retVal = <%d>", is_regular_file);
    return is_regular_file;
#else
    MCL_ERROR_STRING(mcl_no_file_system_message);
    return MCL_FALSE;
#endif
}

mcl_size_t mcl_file_util_get_file_size(void *file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    struct stat file_attributes;
    mcl_size_t file_size = 0;

    MCL_DEBUG_ENTRY("void *file_descriptor = <%p>", file_descriptor);

    if (MCL_OK == _file_util_fstat(file_descriptor, &file_attributes))
    {
        file_size = file_attributes.st_size;
    }

    MCL_DEBUG_LEAVE("retVal = <%u>", file_size);
    return file_size;
#else
    MCL_ERROR_STRING(mcl_no_file_system_message);
    return 0;
#endif
}

mcl_time_t mcl_file_util_get_time_of_last_status_change(void *file_descriptor)
{
#if HAVE_FILE_SYSTEM_
    struct stat file_attributes;
    mcl_time_t time = 0;

    MCL_DEBUG_ENTRY("void *file_descriptor = <%p>", file_descriptor);

    if (MCL_OK == _file_util_fstat(file_descriptor, &file_attributes))
    {
        time = file_attributes.st_ctime;
    }

    MCL_DEBUG_LEAVE("retVal = <%u>", time);
    return time;
#else
    MCL_ERROR_STRING(mcl_no_file_system_message);
    return 0;
#endif
}

#if HAVE_FILE_SYSTEM_
static mcl_error_t _file_util_fstat(void *file_descriptor, struct stat *file_attributes)
{
    mcl_error_t return_code = MCL_FAIL;
    int result;

    MCL_DEBUG_ENTRY("void *file_descriptor = <%p>, struct stat *file_attributes = <%p>", file_descriptor, file_attributes);

#if defined(WIN32) || defined(WIN64)
    result = fstat(_fileno((FILE *)file_descriptor), file_attributes);
#else
    result = fstat(fileno((FILE *)file_descriptor), file_attributes);
#endif

    if (0 == result)
    {
        return_code = MCL_OK;
    }
    else
    {
        MCL_ERROR("Error in retrieving file attributes.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}
#endif
