cmake_minimum_required(VERSION 3.24.2)
project(commutator)

# find_package(msgpack REQUIRED)

# if(NOT msgpack_FOUND)
#     message(
#         SEND_ERROR
#         "Failed to find msgpackc library\n"
#         "https://github.com/msgpack/msgpack-c/blob/c_master/QUICKSTART-C.md"
#     )
#     return()
# endif()

include_directories(include/ deps/)
file(GLOB SOURCES src/*.c deps/*/*.c)

add_executable(commutator ${SOURCES})
target_link_libraries(
    commutator
    # msgpackc-cxx
)
set_property(TARGET commutator PROPERTY C_STANDARD 23)