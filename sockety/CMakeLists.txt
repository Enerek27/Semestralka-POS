
add_library(sockety SHARED)
target_sources(sockety PRIVATE socket.c)
target_compile_options(sockety PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")
target_include_directories(sockety PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )