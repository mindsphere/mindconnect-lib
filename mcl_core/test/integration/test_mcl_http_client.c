/*!
 * @file     test_mcl_http_client.c
 * @brief    Integration tests for mcl http client.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_core/mcl_core_common.h"
#include "mcl_core/mcl_http_response.h"
#include "mcl_core/mcl_http_request.h"
#include "mcl_core/mcl_http_client.h"
#include "mcl_core/mcl_string_util.h"
#include <string.h>

char *mock_server_certificate =
    "-----BEGIN CERTIFICATE-----\n"\
    "MIIDqDCCApCgAwIBAgIEo9uk7jANBgkqhkiG9w0BAQsFADBiMRswGQYDVQQDDBJ3\n"\
    "d3cubW9ja3NlcnZlci5jb20xEzARBgNVBAoMCk1vY2tTZXJ2ZXIxDzANBgNVBAcM\n"\
    "BkxvbmRvbjEQMA4GA1UECAwHRW5nbGFuZDELMAkGA1UEBhMCVUswIBcNMTUwMjEz\n"\
    "MTYzMDA2WhgPMjExNjAxMjAxNjMwMDZaMGIxGzAZBgNVBAMMEnd3dy5tb2Nrc2Vy\n"\
    "dmVyLmNvbTETMBEGA1UECgwKTW9ja1NlcnZlcjEPMA0GA1UEBwwGTG9uZG9uMRAw\n"\
    "DgYDVQQIDAdFbmdsYW5kMQswCQYDVQQGEwJVSzCCASIwDQYJKoZIhvcNAQEBBQAD\n"\
    "ggEPADCCAQoCggEBAIslQgdbmc1UDLjtCsFVeFEPHZWWQsIJkcLCT15QJbLnMPEm\n"\
    "pyiSo07DQ1xQk7WCBOWWBuhJt+MwEAK5QFR/MLf11xcUF26PkO0GO01lEPtS6M8n\n"\
    "pXpNCYKFF+gZ0q1hmxZ08ZQD+6LAPnLx5XUKk/XvjEpKRz01Sza4nUhgc4QDnN+B\n"\
    "CiamQ4fzhbwR6eTPm9Z266RYhK+oPBBN1AxE/+CwO0nV85n9EYimn5kfZqyideCQ\n"\
    "3ZbjPzXgfXctC9cWxKK4eEdfjMRnxxwAVVPCJU59sMv6JjhDV3YKWX4UMevsc6bf\n"\
    "Q/KOgVAoFJuFXbAiS+oE5/JOW0yU9fHngD7QWUMCAwEAAaNkMGIwHQYDVR0OBBYE\n"\
    "FKmWp1h2Zhsn4FP97H977H4izXGkMA8GA1UdEwEB/wQFMAMBAf8wCwYDVR0PBAQD\n"\
    "AgG2MCMGA1UdJQQcMBoGCCsGAQUFBwMBBggrBgEFBQcDAgYEVR0lADANBgkqhkiG\n"\
    "9w0BAQsFAAOCAQEAFW+ARa1RmSjbX82aj5+4hAVb3hiVcEVfDD/yfSwEYP93yNgV\n"\
    "u74z/a3eqrzQKWveKH+CTGaRij1WNAknswrBijSEppNKEebg/J/vpXNwaDNK49oi\n"\
    "8zbNAWPOB/mRK8ogY0qWXUHzt69r6Cm6Jid1TLKlpuJ60REZT8kfQlbXaNjy2g0N\n"\
    "W254k/MBugmBQcKEJ/nfiiZe/1zLkn2JLQAgFf5LIvhGNBS/t7Fz0QiKwyfhyjP3\n"\
    "t0YCdqheMQM8/rFjufCgJDu7IfieKguRoyRRbfSb6/gApANG3ARlLu6SoXK1TjfX\n"\
    "gJvTmKj67HcgyjQgsSTx7r1O/HgxF4+rhpOpxg==\n"\
    "-----END CERTIFICATE-----";

void setUp(void)
{
    
}

void tearDown(void)
{

}

typedef struct
{
    uint16_t counter;
    uint16_t send_until;
} dummy_context_t;

static mcl_size_t dummy_callback(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context);

static const char dummy_payload[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua." \
" Vitae justo eget magna fermentum. Natoque penatibus et magnis dis parturient montes. Eu turpis egestas pretium aenean pharetra. Consequat id porta nibh venenatis cras sed felis." \
" Leo vel fringilla est ullamcorper eget. Fermentum leo vel orci porta. Venenatis a condimentum vitae sapien pellentesque habitant morbi tristique." \
" Scelerisque felis imperdiet proin fermentum leo vel orci. Dignissim convallis aenean et tortor at risus viverra adipiscing at. Semper risus in hendrerit gravida rutrum quisque non." \
" In fermentum posuere urna nec tincidunt praesent semper feugiat nibh. Venenatis lectus magna fringilla urna porttitor rhoncus dolor. Pretium nibh ipsum consequat nisl." \
" Pretium fusce id velit ut tortor pretium viverra suspendisse.";

/**
* GIVEN : Body size for a PUT request is known and payload will be sent using callback.
* WHEN  : HTTP request is sent.
* THEN  : Transfer-Encoding: chunked is not used and HTTP status code is 201.
*/
void test_http_client_01(void)
{
    mcl_http_request_t *http_request = NULL;
    mcl_http_client_t *http_client = NULL;
    E_MCL_HTTP_METHOD http_method = MCL_HTTP_PUT;
    mcl_error_t code;
    mcl_http_client_configuration_t http_client_configuration;
    mcl_size_t body_size = sizeof(dummy_payload) - MCL_NULL_CHAR_SIZE;
    dummy_context_t dummy_context;
    mcl_http_response_t *http_response = NULL;

    // Set http client configuration parameters.
    http_client_configuration.http_request_timeout = 300;
    http_client_configuration.port = 1080;
    http_client_configuration.user_agent = "CONFIGURATION:SERVICE=HTTP_CLIENT_TEST;HTTP_STATUS=201;%";
    http_client_configuration.proxy_hostname = MCL_NULL;
    http_client_configuration.proxy_port = 8888;
    http_client_configuration.proxy_type = MCL_PROXY_HTTP;
    http_client_configuration.certificate = mock_server_certificate;
    http_client_configuration.certificate_is_file = MCL_FALSE;
    http_client_configuration.proxy_domain = MCL_NULL;
    http_client_configuration.proxy_username = MCL_NULL;
    http_client_configuration.proxy_password = MCL_NULL;

    // Set dummy context parameters.
    dummy_context.counter = 0;
    dummy_context.send_until = 4;

    body_size = body_size * dummy_context.send_until;

    code = mcl_http_client_initialize(&http_client_configuration, &http_client);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Could not initialize http client.");

    code = mcl_http_request_initialize(&http_request);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Could not initialize http request.");

    code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_STREAM_CALLBACK, &dummy_callback);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Could not set stream callback.");

    code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_STREAM_DATA, &dummy_context);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Could not set stream data.");

    code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_METHOD, &http_method);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Could not set method.");

    code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_URL, "https://127.0.0.1/http_client_test");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Could not set URL.");

    code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_BODY_SIZE, &body_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Could not set body size.");

    code = mcl_http_client_send(http_client, http_request, &http_response);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Could not send the request.");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_HTTP_STATUS_CODE_CREATED, http_response->status_code, "HTTP 201 should have been returned.");

    // Clean up.
    mcl_http_response_destroy(&http_response);
    mcl_http_request_destroy(&http_request);
    mcl_http_client_destroy(&http_client);
}

static mcl_size_t dummy_callback(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context)
{
    dummy_context_t *dummy_context = (dummy_context_t *) user_context;
    mcl_size_t copy_size;

    if (dummy_context->counter >= dummy_context->send_until)
    {
        return 0;
    }

    ++dummy_context->counter;

    if (sizeof(dummy_payload) > (size * count))
    {
        copy_size = size * count;
    }
    else
    {
        copy_size = sizeof(dummy_payload) - MCL_NULL_CHAR_SIZE;
    }

    memcpy(buffer, dummy_payload, copy_size);

    return copy_size;
}
