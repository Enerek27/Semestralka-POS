
add_executable(klient klient.c)

target_compile_options(klient PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")

add_executable(server server.c)

target_compile_options(server PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")

target_link_libraries(server PRIVATE zdrojove_kody)
target_link_libraries(klient PRIVATE zdrojove_kody)