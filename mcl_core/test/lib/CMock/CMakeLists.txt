add_library(unity_and_cmock STATIC "./src/cmock.c" "./vendor/unity/src/unity.c")

target_include_directories(unity_and_cmock PUBLIC src vendor/unity/src)