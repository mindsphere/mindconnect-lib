# [4.4.0] (2024-09-01)

## Update dependencies and certificates

* cJSON library version is upgraded to 1.7.18.
* Minimum required OpenSSL version is updated to 3.0.13 for users using OpenSSL as security library.
* Minimum required libCurl version is updated to 8.6.0 for users using libCurl as http client.
* QuoVadis Root CA 2 G3" certificate will be retired on 2024-10-15T04:00:00.000Z hence code examples are updated to use existing certificate as well as new certificate i.e. "DigiCert Global Root G2".

# [4.3.4] (2023-04-03)
## Certificate updates
* Insights Hub certificate update information publication

# [4.3.3] (2023-04-03)
## Documentation
* Documentation Changes.

# [4.3.2] (2022-03-19)

## Security

* cJSON library version is upgraded to 1.7.15 since the old version had security vulnerabilities.
* Minimum required OpenSSL version is updated to 1.1.1m for users using OpenSSL as security library and CMake as the build tool.
* Minimum required libCurl version is updated to 7.81.0 for users using libCurl as http client and CMake as the build tool.

# [4.3.1] (2021-05-15)

## Bug Fixes

* Cmake build related issues reported from github repository are fixed.

# [4.3.0] (2021-03-27)

## Features

* MCL Deployment component is introduced for agents to access Deployment Workflow API's.

# [4.2.4] (2020-11-28)

## Features

* MCL is ported to be used by mbedOS applications.

# [4.2.3] (2020-10-10)

## Bug Fixes

* Missing http header is added to requests to data lake on Azure.

# [4.2.2] (2020-09-05)

## Features

* Updated Sony Spresense port for Spresense SDK 2.0.

# [4.2.1] (2020-06-13)

## Features

* Basic http client using tcp sockets and mbedtls for tls is introduced as an alternative to curl-openssl pair.
* MCL is ported to Sony Spresense board using basic http client and mbedtls.

# [4.2.0] (2020-04-16)

## Features

* Data Point Mapping support is added to MCL Connectivity component.

# [4.1.1] (2020-03-19)

## Bug Fixes

* Type mismatch checks added to json utility get functions.

# [4.1.0] (2020-02-03)

## Features

* MCL Data Lake component is released for agents to upload unstructured data.

## Bug Fixes

* Missing C linkage specifications in some headers are added.

# [4.0.1] (2019-12-06)

## Security

* Minimum required OpenSSL version is updated from 1.1.1c to 1.1.1d for users using OpenSSL.

## Features

* mcl_http_client_add_certificate function is added.

## Refactor

* max_http_payload_size parameter of mcl_core_configuration_t structure is removed. Setting it with mcl_core_configuration_set_parameter function will have no effect.
* mcl_core_is_initialized function is removed, hence MCL_NOT_INITIALIZED error code is removed.
* mcl_http_client_configuration_t structure is refactored.

# [4.0.0] (2019-09-13)

## Features

* First release.
