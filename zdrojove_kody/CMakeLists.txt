//Tomi uz vie co tu ma byt

add_library(zdrojove_kody SHARED)
target_sources(zdrojove_kody PRIVATE chodec.c svet.c)
target_compile_options(zdrojove_kody PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")
target_include_directories(zdrojove_kody PUBLIC ..)