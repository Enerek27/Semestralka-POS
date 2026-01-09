
add_library(server_lib SHARED)
target_sources(server_lib PRIVATE pomoc_server.c)
target_compile_options(server_lib PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")
target_include_directories(server_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )