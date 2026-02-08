add_library(mylib STATIC
    mylib.cpp
)

target_include_directories(mylib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(mylib PUBLIC cxx_std_20)
