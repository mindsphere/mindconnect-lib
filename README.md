# MindConnect Library (MCL) 4.x.x

## Document Content
This is the introductory document for MindConnect Library, abbreviated as MCL.
Please check MCL reference documentation for details. MCL reference documentation can be accessed online 
or a local copy can be generated when building MCL (see MCL Build Options).

## Introduction
MindConnect Library (MCL), is a cross-platform C library wrapping API's to which clients or agents are allowed to access the platform. The connection to platform is secured using TLS v1.2.

MindConnect Library is distributed as source code. The source code is organized as a core component and extensions to the core component.

Onboarding, key rotation and access token retrieval functionalities will be available to agents with the **Core** component. 
Including **mcl_core.h** in agent source code will enable all core component functionality.

Data exchange and data point mapping functionalities will be available to agents with **Connectivity** component which is an extension to **Core** component.
Including **mcl_connectivity.h** in agent source code will enable all connectivity component functionality together with core component functionality.

Agents will be able to upload unstructured data to Integrated Data Lake with **Data Lake** component of MCL 
which is also an extension to **Core** component and it is independent of the **Connectivity** component. 
Including **mcl_data_lake.h** in agent source code will enable all data lake component functionality together with core component functionality.

Agents will be able to get and update any workflows assigned to them with **Deployment** component of MCL, an extension to **Core** component. 
Including **mcl_deployment.h** in agent source code will enable all deployment component functionality together with core component functionality.

MCL build process will generate separate output binaries for each component. 
Agents willing to exploit only the core functionality can do so by building and including only the core component. 
Agents willing to exploit any extension component functionality can do so by building and including that extension component together with 
any components that it depends on.

## Dependencies
-   CMake is used for cross-platform building of MCL although developers can choose to use other options to build MCL. 
	CMake version 3.10 or later is required. 

-   LibCurl is used as the default HTTP client in MCL but developers have the option to easily replace LibCurl with other HTTP client implementations.
	Make sure LibCurl is installed on your system before building MCL if you are going to use LibCurl as the HTTP client. 
	Steps to build LibCurl for your target system are described in the following sections of this document.
	Developers are encouraged to use newer versions of LibCurl but note that MCL is tested with LibCurl version 7.81.0. 

	Please make sure your HTTP client implementation is configured to use TLS v1.2 implementation.

-   OpenSSL is used as the default TLS and cryptography library but developers have the option to easily replace OpenSSL with other implementations. 
	Make sure OpenSSL is installed on your system before building MCL if you are going to use OpenSSL as TLS and cryptography library. 
	Steps to build OpenSSL are described in the following sections of this document.
	Developers are encouraged to use newer versions of OpenSSL 1.1.x but note that MCL is tested with OpenSSL version 1.1.1m. 
	
	One of following ciphers must be supported and configured for SSL handshaking:

			AES128-SHA256
			AES256-SHA256
			AES128-GCM-SHA256
			AES256-GCM-SHA384
			ECDHE-RSA-AES128-SHA256
			ECDHE-RSA-AES256-SHA384
			ECDHE-RSA-AES128-GCM-SHA256
			ECDHE-RSA-AES256-GCM-SHA384

	In order to verify the correctness of the Platforms's certificate
	please enable host and peer verification.
	
-   If you want to have unit and integration test executables also built
	with cmake, please ensure that ruby is installed and available on
	the path. If you do not have ruby on your path no test executables will
	be generated.

## Replacing MCL Modules
Agent developers can provide custom implementations for the following modules and configure MCL build to use custom implementations in place of the default implementations.

### HTTP Client
LibCurl is set as the default HTTP client library in MCL which can be replaced by implementing the functions declared in **"mcl_core/include/mcl_core/mcl_http_client.h"**.
Default implementation is located in **"mcl_core/src/http_client/curl/http_client_libcurl.c"**.
Note that default http client implementation depends on OpenSSL for TLS.

You may place your implementation in:

**a)** a subfolder under **"mcl_core/src/http_client/"** so that the directory structure looks like:
```
+-- mcl_core
    +-- src
        +-- http_client
            +-- curl
            +-- <your_implementation_folder>
                +-- <your_source_file(s)>
                +-- CMakeLists.txt
```
In this case, MCL can be configured to use the alternative implementation by setting **-DMCL_HTTP_CLIENT=<your_implementation_folder>** when building MCL.

**b)** any place out of MCL project so that the directory structure looks like:
```
+-- mcl_core
    +-- src
        +-- http_client
            +-- curl
            
+-- <any_place_out_of_MCL>
    +-- <your_implementation_folder>
        +-- <your_source_file(s)>
        +-- CMakeLists.txt
```
In this case, MCL can be configured to use the alternative implementation by setting **-DMCL_HTTP_CLIENT=<absolute_path_to_your_implementation_folder>** when building MCL.

If you are using CMake to build MCL, you also need to provide CMakeLists.txt file within your alternative implementation directory. CMakeLists.txt file must set the parameters
**MCL_HTTP_CLIENT_INCLUDE_DIRECTORIES**, **MCL_HTTP_CLIENT_LIBRARIES** and **MCL_HTTP_CLIENT_SOURCES** for **PARENT_SCOPE**. Typical CMakeLists.txt would look like:

```cmake
SET(MCL_HTTP_CLIENT_INCLUDE_DIRECTORIES <directory_to_include> <another_directory_to_include> PARENT_SCOPE)
SET(MCL_HTTP_CLIENT_LIBRARIES <library_to_be_linked> <another_library_to_be_linked> PARENT_SCOPE)
SET(MCL_HTTP_CLIENT_SOURCES <your_source_file> <your_another_source_file> PARENT_SCOPE)
```

If you are using CMake to build MCL, and your alternative implementation depends on another library, do not forget to add the directory
(with contents including headers and the binaries) of that library to **CMAKE_PREFIX_PATH** option when building MCL. See section **Building MCL** for details.

### Crypto
OpenSSL is set as the default cryptography library in MCL which can be replaced by implementing the functions declared in **"mcl_core/src/security.h"**.
Default implementation is located in **"mcl_core/src/crypto/openssl/security_libcrypto.c"**.

You may place your implementation in:

**a)** a subfolder under **"mcl_core/src/crypto/"** so that the directory structure looks like:

```
+-- mcl_core
    +-- src
        +-- crypto
            +-- openssl
            +-- <your_implementation_folder>
                +-- <your_source_file(s)>
                +-- CMakeLists.txt
```
In this case, MCL can be configured to use the alternative implementation by setting **-DMCL_CRYPTO=<your_implementation_folder>** when building MCL.

**b)** any place out of MCL project so that the directory structure looks like:

```
+-- mcl_core
    +-- src
        +-- crypto
            +-- openssl
            
+-- <any_place_out_of_MCL>
    +-- <your_implementation_folder>
        +-- <your_source_file(s)>
        +-- CMakeLists.txt
```
In this case, MCL can be configured to use the alternative implementation by setting **-DMCL_CRYPTO=<absolute_path_to_your_implementation_folder>** when building MCL.

If you are using CMake to build MCL, you also need to provide CMakeLists.txt file within your alternative implementation directory. CMakeLists.txt file must set the parameters
**MCL_CRYPTO_INCLUDE_DIRECTORIES**, **MCL_CRYPTO_LIBRARIES** and **MCL_CRYPTO_SOURCES** for **PARENT_SCOPE**. Typical CMakeLists.txt would look like:

```cmake
SET(MCL_CRYPTO_INCLUDE_DIRECTORIES <directory_to_include> <another_directory_to_include> PARENT_SCOPE)
SET(MCL_CRYPTO_LIBRARIES <library_to_be_linked> <another_library_to_be_linked> PARENT_SCOPE)
SET(MCL_CRYPTO_SOURCES <your_source_file> <your_another_source_file> PARENT_SCOPE)
```

If you are using CMake to build MCL, and your alternative implementation depends on another library, do not forget to add the directory
(with contents including headers and the binaries) of that library to **CMAKE_PREFIX_PATH** option when building MCL. See section **Building MCL** for details.

### File Utility
MCL uses standard C Library as the default file utility for file operations which can be replaced by implementing the functions declared in **"mcl_core/include/mcl_core/mcl_file_util.h"**.
Default implementation is located in **"mcl_core/src/file_util/standard/file_util.c"**.

You may place your implementation in:

**a)** a subfolder under **"mcl_core/src/file_util/"** so that the directory structure looks like:
```
+-- mcl_core
    +-- src
        +-- file_util
            +-- standard
            +-- <your_implementation_folder>
                +-- <your_source_file(s)>
                +-- CMakeLists.txt
```
In this case, MCL can be configured to use the alternative implementation by setting **-DMCL_FILE_UTIL=<your_implementation_folder>** when building MCL.

**b)** any place out of MCL project so that the directory structure looks like:
```
+-- mcl_core
    +-- src
        +-- file_util
            +-- standard
            
+-- <any_place_out_of_MCL>
    +-- <your_implementation_folder>
        +-- <your_source_file(s)>
        +-- CMakeLists.txt
```
In this case, MCL can be configured to use the alternative implementation by setting **-DMCL_FILE_UTIL=<absolute_path_to_your_implementation_folder>** when building MCL.

If you are using CMake to build MCL, you also need to provide CMakeLists.txt file within your alternative implementation directory. CMakeLists.txt file must set the parameters
**MCL_FILE_UTIL_INCLUDE_DIRECTORIES**, **MCL_FILE_UTIL_LIBRARIES** and **MCL_FILE_UTIL_SOURCES** for **PARENT_SCOPE**. Typical CMakeLists.txt would look like:

```cmake
SET(MCL_FILE_UTIL_INCLUDE_DIRECTORIES <directory_to_include> <another_directory_to_include> PARENT_SCOPE)
SET(MCL_FILE_UTIL_LIBRARIES <library_to_be_linked> <another_library_to_be_linked> PARENT_SCOPE)
SET(MCL_FILE_UTIL_SOURCES <your_source_file> <your_another_source_file> PARENT_SCOPE)
```

If you are using CMake to build MCL, and your alternative implementation depends on another library, do not forget to add the directory
(with contents including headers and the binaries) of that library to **CMAKE_PREFIX_PATH** option when building MCL. See section **Building MCL** for details.

### Memory
MCL uses standard C Library as the default utility for memory operations which can be replaced by implementing the functions declared in **"mcl_core/include/mcl_core/mcl_memory.h"**.
Default implementation is located in **"mcl_core/src/memory/standard/memory.c"**.

You may place your implementation in:

**a)** a subfolder under **"mcl_core/src/memory/"** so that the directory structure looks like:
```
+-- mcl_core
    +-- src
        +-- memory
            +-- standard
            +-- <your_implementation_folder>
                +-- <your_source_file(s)>
                +-- CMakeLists.txt
```
In this case, MCL can be configured to use the alternative implementation by setting **-DMCL_MCL_MEMORY=<your_implementation_folder>** when building MCL.

**b)** any place out of MCL project so that the directory structure looks like:
```
+-- mcl_core
    +-- src
        +-- memory
            +-- standard
            
+-- <any_place_out_of_MCL>
    +-- <your_implementation_folder>
        +-- <your_source_file(s)>
        +-- CMakeLists.txt
```
In this case, MCL can be configured to use the alternative implementation by setting **-DMCL_MEMORY=<absolute_path_to_your_implementation_folder>** when building MCL.

If you are using CMake to build MCL, you also need to provide CMakeLists.txt file within your alternative implementation directory. CMakeLists.txt file must set the parameters
**MCL_MEMORY_INCLUDE_DIRECTORIES**, **MCL_MEMORY_LIBRARIES** and **MCL_MEMORY_SOURCES** for **PARENT_SCOPE**. Typical CMakeLists.txt would look like:

```cmake
SET(MCL_MEMORY_INCLUDE_DIRECTORIES <directory_to_include> <another_directory_to_include> PARENT_SCOPE)
SET(MCL_MEMORY_LIBRARIES <library_to_be_linked> <another_library_to_be_linked> PARENT_SCOPE)
SET(MCL_MEMORY_SOURCES <your_source_file> <your_another_source_file> PARENT_SCOPE)
```

If you are using CMake to build MCL, and your alternative implementation depends on another library, do not forget to add the directory
(with contents including headers and the binaries) of that library to **CMAKE_PREFIX_PATH** option when building MCL. See section **Building MCL** for details.

## Building MCL
This section will guide you to build MCL as a dynamic library from source code using CMake in Linux environment and also show you how to import and use MCL inside an example agent application.
Although the steps are described for Linux environment, they are expected to be informative enough for other environments as well.

Default implementations for replaceable modules (i.e. http client, crypto, memory and file utility) will be used. 
If you intend to use alternative implementation for any of the replaceable modules see section **Replacing MCL Modules**.

Both the core component and the connectivity extension will be built. See **MCL Build Options** to select components to build.

These are the steps of the build process in order:

### Building OpenSSL
Download OpenSSL package [openssl-1.1.1m.tar.gz](https://www.openssl.org/source/) and in your download directory run the following commands where
**\<OpenSSL_Install_Directory\>** refers to the directory you want to install OpenSSL:

```sh
$ tar -xvzf openssl-1.1.1m.tar.gz
$ cd openssl-1.1.1m
$ ./config --prefix=<OpenSSL_Install_Directory> -Wl,-rpath=<OpenSSL_Install_Directory>/lib shared -fPIC
$ sudo make install
```

Now, there must be a folder named **openssl** in **\<OpenSSL_Install_Directory\>/include** and shared objects named **libcrypto.so** and **libssl.so** in **\<OpenSSL_Install_Directory\>/lib**.

### Building Libcurl
Download libcurl package [curl-7.81.0.tar.gz](https://curl.haxx.se/download/) and in your download directory run the following commands where
**\<libcurl_Install_Directory\>** refers to the directory you want to install libcurl:

```sh
$ tar -xvzf curl-7.81.0.tar.gz
$ cd curl-7.81.0
$ LDFLAGS="-Wl,-R<OpenSSL_Install_Directory>/lib" ./configure --enable-http --with-ssl=<OpenSSL_Install_Directory> --prefix=<libcurl_Install_Directory> --without-libssh2 --disable-ftp --disable-tftp --disable-file --disable-ldap --disable-rtsp --disable-dict --disable-telnet --disable-pop3 --disable-imap --disable-smb --disable-scp --disable-sftp --disable-smtp --disable-gopher --disable-manual
$ sudo make install
```

Now, there must be a folder named **curl** in **\<libcurl_Install_Directory\>/include** and shared object named **libcurl.so** in **\<libcurl_Install_Directory\>/lib**.

### Building MCL
After the dependencies of MCL are built, to build MCL, run the following commands where 

**\<MCL_Source_Directory>** refers to the directory of MCL source code (i.e. directory in which README.md, top level CMakeLists.txt and subfolders for each component exist),  
**\<MCL_Build_Directory>** refers to the intermediate directory used during build process,  
**\<MCL_Install_Directory>** refers to the directory in which build artifacts are located.  

```sh
$ mkdir -p <MCL_Build_Directory>
$ cd <MCL_Build_Directory>
$ cmake -DCMAKE_PREFIX_PATH="<OpenSSL_Install_Directory>;<libcurl_Install_Directory>" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX:PATH=<MCL_Install_Directory> -DMCL_STATICLIB=OFF -DMCL_LOG_LEVEL=MCL_LOG_LEVEL_NONE <MCL_Source_Directory>
$ cmake --build . --clean-first --target install
```

Now, there must be folders named **mcl_core**, **mcl_connectivity**, **mcl_data_lake** and **mcl_deployment** containing MCL headers in **\<MCL_Install_Directory\>/include** 
and shared objects named **libmcl_core.so**, **libmcl_connectivity.so**, **libmcl_data_lake.so** and **libmcl_deployment.so** in **\<MCL_Install_Directory\>/lib**.

#### MCL Build Options
You can build MCL with different configurations with the options listed in the table below:

Variable | Default Value | Description
------------ | ------------- | -------------
MCL_STATICLIB | OFF | If set to ON, MCL components are built as static library.
MCL_DOC | OFF | If set to ON, MCL reference documentation is also built.
MCL_TEST | OFF | If set to ON and if ruby is found in path MCL is built with tests.
MCL_CONNECTIVITY | ON | If set to ON, MCL Connectivity component will also be built.
MCL_DATA_LAKE | ON | If set to ON, MCL Data Lake component will also be built.
MCL_DEPLOYMENT | ON | If set to ON, MCL Deployment component will also be built.
MCL_CRYPTO | "openssl" | See "Replacing MCL Modules" section.
MCL_HTTP_CLIENT | "curl" | See "Replacing MCL Modules" section.
MCL_FILE_UTIL | "standard" | See "Replacing MCL Modules" section.
MCL_MEMORY | "standard" | See "Replacing MCL Modules" section.
MCL_LOG_LEVEL | MCL_LOG_LEVEL_INFO | MCL logging level. See **MCL Logging** section.

#### MCL Logging
MCL logging level can be configured when building MCL by setting **MCL_LOG_LEVEL** option to one of the values listed in the table below:

| MCL Logging Level | Description
| ---------- | ----------  
| MCL_LOG_LEVEL_VERBOSE | Extensive details of operation are logged together with MCL_LOG_LEVEL_DEBUG level logs.
| MCL_LOG_LEVEL_DEBUG | Diagnostically helpful information are logged together with MCL_LOG_LEVEL_INFO level logs.
| MCL_LOG_LEVEL_INFO | Information for the stages of operation are logged together with MCL_LOG_LEVEL_WARN level logs.
| MCL_LOG_LEVEL_WARN | Warnings are logged together with MCL_LOG_LEVEL_ERROR level logs.
| MCL_LOG_LEVEL_ERROR | Operational errors are logged together with MCL_LOG_LEVEL_FATAL level logs.
| MCL_LOG_LEVEL_FATAL | Only fatal errors are logged.
| MCL_LOG_LEVEL_NONE | Nothing is logged.

Scope of logging can be reduced in agent application code from the level set at build time but can not be extended. See reference documentation for details.
 
### Building Custom Agent Application
MCL source code is distributed with several example agent applications. See **Examples** section for details.
To build an example agent application, you can copy the source code of one of the examples to a directory, referred as **\<Custom_Agent_Project_Directory\>**, 
and create a CMakeLists.txt file in that directory with the content below: 

```cmake
CMAKE_MINIMUM_REQUIRED(VERSION 3.10 FATAL_ERROR)
PROJECT(CustomAgentApplication LANGUAGES C)
SET(CMAKE_C_STANDARD 99)
SET(CMAKE_C_STANDARD_REQUIRED ON)
FILE(GLOB SOURCES *.c)
LIST(APPEND AGENT_SOURCES ${SOURCES})
SET(MCL_CORE "<MCL_Install_Directory>/lib/libmcl_core.so")
SET(MCL_CONNECTIVITY "<MCL_Install_Directory>/lib/libmcl_connectivity.so")
SET(MCL_INCLUDE_DIRECTORY "<MCL_Install_Directory>/include/")

SET(AGENT_OUTPUT_DIR ${CMAKE_BINARY_DIR}/build)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${AGENT_OUTPUT_DIR})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${AGENT_OUTPUT_DIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${AGENT_OUTPUT_DIR})
ADD_EXECUTABLE(${PROJECT_NAME} ${AGENT_SOURCES})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${MCL_INCLUDE_DIRECTORY})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MCL_CORE} ${MCL_CONNECTIVITY})
```

Now, you can compile your agent application by running the following commands where **\<Custom_Agent_Build_Directory\>** refers to the build directory for your project:

```sh
$ mkdir -p <Custom_Agent_Build_Directory>
$ cd <Custom_Agent_Build_Directory>
$ cmake <Custom_Agent_Project_Directory>
$ cmake --build . --clean-first
```

You will find agent application executable named **\<CustomAgentApplication\>** in **\<Custom_Agent_Build_Directory>\**.

### Building MCL In Specific Environments
Although the default build tool is CMake, MCL is ported to be built as part of other SDK in specific environments. You can build MCL as part of an mbedOS application by 
importing MCL as a library to your application in mbedOS development environment. You can integrate MCL to Sony Spresense environment to be built together 
with Sony Spresense SDK by following the document <a href = "./SonySpresenseGuide.md" target="_blank"><u>Sony Spresense Guide</u></a> provided with MCL source code.

## Accessing Reference Documentation
You can access reference documentation online from https://developer.mindsphere.io/resources/mindconnect-lib-v4/doxygen/html/index.html.
Alternatively, you can generate a local copy of the reference documentation using Doxygen.

## Developing Agent Application Using MCL
For an agent application to use MCL, first step would be to initialize core configuration data structure (**mcl_core_configuration_t**) 
and set the values of configuration parameters. See **mcl_core_configuration.h** for details.

Core configuration data structure is then used to initialize MCL core component. The core component instance (**mcl_core_t**) 
is later used to onboard the agent, rotate keys when required and retrieve access token for connectivity services. 
The core component handle is also required to initialize MCL connectivity extension. See **mcl_core.h** for details.

To access to connectivity services, the agent must initialize MCL connectivity extension component.
The connectivity extension instance (**mcl_connectivity_t**) is later used to exchange data. 
See **mcl_connectivity.h** for details.

The agent can exchange predefined data types or any custom data type with MCL. The predefined data types are 
data source configuration, timeseries, event and file. See **mcl_data_source_configuration.h**, **mcl_timeseries.h**, 
**mcl_event.h** and **mcl_file.h** for details on how to create and exchange predefined data types.
For custom data type, see **mcl_custom_data.h**. 

Note that, before exchanging timeseries data, data source configuration which describes the timeseries data has to be uploaded first and then the data points in the data source configuration have to mapped to the corresponding properties of the asset. See **mcl_mapping.h** for 
data point mapping details.

Agents have the option to exchange each data item a a single or they have the option to exchange multiple data items 
independent of their types together in a store (**mcl_store_t). See **mcl_store.h** for details.

## Examples
You can find agent application examples using MCL in **/examples** folder of each component of MCL distribution.

### Example : Onboard
This example onboards and Agent.
Check <MCL_Source_Directory>/mcl_core/examples/onboard.c file for the implementation.

### Example : Data Source Configuration Upload
This example uploads a data source configuration of the Agent.
Check <MCL_Source_Directory>/mcl_connectivity/examples/dsc_upload.c file for the implementation.

### Example : Data Point Mapping
This example creates mappings between the data points in the data source configuration 
of the Agent and the properties of the asset.
Check <MCL_Source_Directory>/mcl_connectivity/examples/create_mapping.c file for the implementation.

### Example : Timeseries Upload
This example uploads timeseries data of the Agent with for given data source configuration.
Check <MCL_Source_Directory>/mcl_connectivity/examples/timeseries_upload.c file for the implementation.

### Example : File Upload
This example agent uploads a file.
Check <MCL_Source_Directory>/mcl_connectivity/examples/file_upload.c file for the implementation.

### Example : Event Upload
This example agent uploads an event.
Check <MCL_Source_Directory>/mcl_connectivity/examples/event_upload.c file for the implementation.

### Example : Store Upload
This example agent uploads a store containing file and timeseries items.
Check <MCL_Source_Directory>/mcl_connectivity/examples/event_upload.c file for the implementation.

### Example : Data Lake Object Upload
This example agent uploads an object to Integrated Data Lake.
Check <MCL_Source_Directory>/mcl_data_lake/examples/data_lake_upload.c file for the implementation.

### Example : Deployment Workflow
This example agent gets and updates deployment workflow assigned to it.
Check <MCL_Source_Directory>/mcl_deployment/examples/manage_deployment.c file for the implementation.

## Licensing
Please see the files called LICENSE.md and ReadMe_OSS.htm.
