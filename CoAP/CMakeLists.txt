cmake_minimum_required(VERSION 3.10)
project(Coap_IMD VERSION 1.0)

find_package(libcoap REQUIRED)
find_package(OpenSSL REQUIRED)

set(CFLAGS_WARNINGS 
-pedantic 
-Wall
-Wcast-qual 
-Wextra 
-Wformat-security 
-Winline 
-Wmissing-declarations 
-Wmissing-prototypes 
-Wnested-externs 
-Wpointer-arith 
-Wshadow 
-Wstrict-prototypes 
-Wswitch-default 
-Wswitch-enum 
-Wunused 
-Wwrite-strings)


add_executable(coap-server-imd coap-server-imd.c)
target_link_libraries(coap-server-imd PRIVATE libcoap::coap-3 pthread)
target_compile_options(coap-server-imd PRIVATE ${CFLAGS_WARNINGS} -D_GNU_SOURCE)
target_compile_features(coap-server-imd PUBLIC c_std_11)

add_executable(coap-client-imd coap-client-imd.c)
target_link_libraries(coap-client-imd PRIVATE libcoap::coap-3)
target_compile_options(coap-client-imd PRIVATE ${CFLAGS_WARNINGS} -D_GNU_SOURCE)
target_compile_features(coap-client-imd PUBLIC c_std_11)

