cmake_minimum_required(VERSION 3.14)

# CogLlama Demo Tool
add_executable(llama-cogllama-demo cogllama-demo.c)

target_include_directories(llama-cogllama-demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/cogllama-core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ggml/include
)

target_link_libraries(llama-cogllama-demo PRIVATE
    cogllama-core
    ggml
)

target_compile_features(llama-cogllama-demo PRIVATE c_std_99)
