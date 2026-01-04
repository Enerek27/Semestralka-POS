
add_library(zdielanaPamat SHARED)
target_sources(zdielanaPamat PRIVATE pipe.c)
target_compile_options(zdielanaPamat PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")
target_include_directories(zdielanaPamat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )