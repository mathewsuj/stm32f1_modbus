cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project("stm32f1_modbus" C CXX ASM)

include(cmake/st-project.cmake)

# Enable exception handling
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti")

add_executable(${PROJECT_NAME})
add_st_target_properties(${PROJECT_NAME})

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 13.1)
    target_link_options(${PROJECT_NAME} PRIVATE "LINKER:--no-warn-rwx-segments")
endif()