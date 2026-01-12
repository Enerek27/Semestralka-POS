
add_executable(klient klient.c)

target_compile_options(klient PRIVATE "-g" "-std=c11" "-Wall" "-Wextra" "-Wpedantic")


target_link_libraries(klient PRIVATE zdrojove_kody)

target_link_libraries(klient PRIVATE sockety)

target_link_libraries(klient PRIVATE zdielanaPamat)

target_link_libraries(klient PRIVATE klient_lib)