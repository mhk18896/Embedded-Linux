cmake_minimum_required(VERSION 2.8)

project(capp)

add_executable(capp capp.c)

install(TARGETS capp DESTINATION bin)