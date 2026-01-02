//Tomi uz vie co tu ma byt

add_executable(klient)
target_sources(klient PRIVATE chodec.c svet.c)
target_compile_options(client_server PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")

add_executable(server)
target_sources(server PRIVATE chodec.c svet.c)
target_compile_options(client_server PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")

target_link_libraries(server PRIVATE zdrojove_kody)
target_link_libraries(klient PRIVATE zdrojove_kody)