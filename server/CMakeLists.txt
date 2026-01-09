

add_executable(server server.c)
add_executable(server_pipe server_pipe.c)
target_compile_options(server PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")

target_link_libraries(server PRIVATE zdrojove_kody)

target_link_libraries(server PRIVATE sockety)

target_link_libraries(server PRIVATE zdielanaPamat)
target_link_libraries(server PRIVATE server_lib)



target_compile_options(server_pipe PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")

target_link_libraries(server_pipe PRIVATE zdrojove_kody)

target_link_libraries(server_pipe PRIVATE sockety)
target_link_libraries(server_pipe PRIVATE zdielanaPamat)
target_link_libraries(server_pipe PRIVATE server_lib)