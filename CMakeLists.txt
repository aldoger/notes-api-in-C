cmake_minimum_required(VERSION 3.23)

project(NOTES_APP)

add_executable(Server)

target_sources(Server
  PRIVATE
    server.c
)

add_library()
