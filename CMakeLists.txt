cmake_minimum_required(VERSION 3.28)
project(tikshoret_targil_1 C)

set(CMAKE_C_STANDARD 11)

add_executable(tikshoret_targil_1
        markov_chain.c
        tweets_generator.c
        linked_list.c)
