cmake_minimum_required(VERSION 3.29)
project(TP_01_Artabaz C)

set(CMAKE_C_STANDARD 11)

add_executable(TP_01_Artabaz main.c
        TOF.c
        TOF.h
        tovs.c
        tovs.h
        constants.h)
