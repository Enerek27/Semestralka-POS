
add_library(klient_lib SHARED)
target_sources(klient_lib PRIVATE pomoc_klient.c)
target_compile_options(klient_lib PRIVATE "-g" "-std=c11" "-Wall" "-Wextra" "-Wpedantic")
target_include_directories(klient_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )