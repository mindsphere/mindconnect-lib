# [4.3.0] (2021-03-27)

## Features

* MCL Deployment component is introduced for agents to access Deployment Workflow API of MindSphere.

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
