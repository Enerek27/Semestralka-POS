cmake_minimum_required(VERSION 3.22)

project (
  SMESTRALKA-POS
VERSION 0.1.0
DESCRIPTION "Commands and their usage with processes and threads"
LANGUAGES C
)
add_subdirectory(server)
add_subdirectory(klient)
add_subdirectory(zdrojove_kody)
add_subdirectory(sockety)
add_subdirectory(zdielanaPamat)
add_subdirectory(server_lib)
add_subdirectory(klient_lib)
set(CMAKE_BUILD_TYPE Debug)