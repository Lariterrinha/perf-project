cmake_minimum_required(VERSION 3.10)
project(MeuProjetoOpenCV)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(meu_app
    projet_24_v1.cpp
    my_functions.cpp
)

target_compile_options(meu_app PRIVATE
    -O3
    -march=native
    -mtune=native
    ${OpenMP_CXX_FLAGS}
)

target_link_libraries(meu_app PRIVATE 
    ${OpenCV_LIBS}
    OpenMP::OpenMP_CXX
)